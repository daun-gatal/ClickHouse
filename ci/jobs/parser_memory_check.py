#!/usr/bin/env python3
"""
Parser AST Memory Profiler CI Check

Compares parser AST memory allocations between the PR build and master build.
Uses jemalloc heap profiles (not stats.allocated) as the source of truth.
Results appear directly in the praktika CI report (json.html) as test cases.
"""

import glob
import os
import re
import subprocess
from pathlib import Path

from ci.praktika.result import Result
from ci.praktika.utils import Shell, Utils

TEMP_DIR = f"{Utils.cwd()}/ci/tmp"
QUERIES_FILE = f"{Utils.cwd()}/utils/parser-memory-profiler/test_queries.txt"
MASTER_PROFILER_URL = "https://clickhouse-builds.s3.us-east-1.amazonaws.com/master/aarch64/parser_memory_profiler"

# Threshold: changes below this are considered OK (noise)
CHANGE_THRESHOLD_BYTES = 32

# Frame prefixes to strip from stack tops (jemalloc/malloc internals)
NOISE_FRAME_PREFIXES = (
    "je_",
    "imalloc",
    "iallocztm",
    "ialloc",
    "ifree",
    "arena_",
    "tcache_",
    "prof_",
    "malloc",
    "calloc",
    "realloc",
    "free",
    "operator new",
    "operator delete",
    "__libc_",
    "__GI_",
    "_int_malloc",
    "_int_free",
    "isalloc",
    "sdallocx",
    "rallocx",
    "mallocx",
)


def download_master_binary(dest_path: str) -> bool:
    """Download master parser_memory_profiler from S3."""
    print(f"Downloading master binary from {MASTER_PROFILER_URL}")
    return Shell.check(
        f"wget -nv -O {dest_path} {MASTER_PROFILER_URL} && chmod +x {dest_path}"
    )


def parse_symbolized_heap(sym_file: str) -> dict:
    """
    Parse a symbolized .heap.sym file produced by symbolizeHeapProfile().

    Format:
        --- symbol
        binary=/path/to/binary
        0x1234 DB::parseQuery--DB::ParserSelectQuery::parseImpl
        ---
        --- heap
        heap_v2/1
          t*: <curobjs>: <curbytes> [<accumobjs>: <accumbytes>]
        @ 0x1234 0x5678
          t*: <curobjs>: <curbytes> [<accumobjs>: <accumbytes>]

    Returns dict: { "stack_key" -> { "bytes": int, "stack": "symbolized;frames" } }
    """
    if not os.path.exists(sym_file):
        print(f"Warning: symbolized file not found: {sym_file}")
        return {}

    with open(sym_file, "r") as f:
        content = f.read()

    # Parse symbol table: address -> "symbol1--symbol2"
    symbols = {}
    in_symbol_section = False
    in_heap_section = False
    lines = content.split("\n")

    for line in lines:
        if line.strip() == "--- symbol":
            in_symbol_section = True
            continue
        if line.strip() == "---" and in_symbol_section:
            in_symbol_section = False
            continue
        if line.strip() == "--- heap":
            in_heap_section = True
            continue

        if in_symbol_section:
            if line.startswith("binary="):
                continue
            # Format: 0x1234ABCD symbolname--inlinename
            parts = line.strip().split(" ", 1)
            if len(parts) == 2 and parts[0].startswith("0x"):
                addr = parts[0].lower()
                symbols[addr] = parts[1]

    # Parse heap section: extract stacks with byte counts
    stacks = {}
    current_addrs = None

    for line in lines:
        if not in_heap_section:
            if line.strip() == "--- heap":
                in_heap_section = True
            continue

        if line.startswith("@"):
            # Stack trace line: @ 0x1234 0x5678 ...
            addr_strs = line[1:].strip().split()
            current_addrs = []
            for a in addr_strs:
                # Normalize to lowercase hex with 0x prefix
                a_clean = a.lower()
                if not a_clean.startswith("0x"):
                    a_clean = "0x" + a_clean
                current_addrs.append(a_clean)

        elif current_addrs is not None and line.strip().startswith("t*:"):
            # Thread stats line: t*: curobjs: curbytes [accumobjs: accumbytes]
            match = re.match(r"\s*t\*:\s*(\d+):\s*(\d+)\s*\[", line)
            if match:
                curbytes = int(match.group(2))
                if curbytes > 0:
                    # Symbolize the stack
                    sym_frames = []
                    for addr in current_addrs:
                        # Try exact match, then try addr-1 (caller address fix)
                        sym = symbols.get(addr, "")
                        if not sym:
                            # Try without leading zeros
                            addr_int = int(addr, 16)
                            addr_minus1 = hex(addr_int - 1)
                            sym = symbols.get(addr_minus1, addr)
                        sym_frames.append(sym if sym else addr)

                    stack_key = ";".join(current_addrs)
                    stacks[stack_key] = {
                        "bytes": curbytes,
                        "frames": sym_frames,
                    }
            current_addrs = None

    return stacks


def filter_stack_frames(frames: list) -> list:
    """Strip leading jemalloc/malloc/libc frames, keep from first ClickHouse frame."""
    filtered = []
    found_clickhouse = False
    for frame in frames:
        if not found_clickhouse:
            # Check if this is a noise frame
            is_noise = False
            for prefix in NOISE_FRAME_PREFIXES:
                if frame.startswith(prefix):
                    is_noise = True
                    break
            if is_noise:
                continue
            found_clickhouse = True
        filtered.append(frame)
    return filtered if filtered else frames  # fallback to full stack if all filtered


def format_stack(frames: list) -> str:
    """Format symbolized frames as a compact one-liner."""
    filtered = filter_stack_frames(frames)
    # Split multi-frame symbols (separated by --) and flatten
    flat = []
    for f in filtered:
        parts = f.split("--")
        flat.extend(parts)
    # Take the most relevant frames (bottom-up, skip very deep ones)
    if len(flat) > 6:
        return " > ".join(flat[:6]) + " > ..."
    return " > ".join(flat)


def compute_diff(stacks_before: dict, stacks_after: dict) -> tuple:
    """
    Compute per-stack byte diffs between before and after heap profiles.
    Returns (total_diff, list of (diff_bytes, formatted_stack) sorted by |diff| desc).
    """
    all_keys = set(stacks_before.keys()) | set(stacks_after.keys())
    diffs = []
    total = 0

    for key in all_keys:
        before_bytes = stacks_before.get(key, {}).get("bytes", 0)
        after_bytes = stacks_after.get(key, {}).get("bytes", 0)
        diff = after_bytes - before_bytes
        if diff != 0:
            frames = stacks_after.get(key, stacks_before.get(key, {})).get(
                "frames", [key]
            )
            diffs.append((diff, format_stack(frames)))
            total += diff

    diffs.sort(key=lambda x: -abs(x[0]))
    return total, diffs


def run_profiler_with_heap(
    binary_path: str, query: str, profile_prefix: str, symbolize: bool = True
) -> dict:
    """
    Run parser_memory_profiler with heap profiling on a single query.
    Returns dict with keys: jemalloc_diff, heap_diff, stack_diffs, error
    """
    env = os.environ.copy()
    env["MALLOC_CONF"] = "prof:true,prof_active:true,prof_thread_active_init:true,lg_prof_sample:0"

    args = [binary_path, "--profile", profile_prefix]
    if symbolize:
        args.append("--symbolize")

    try:
        result = subprocess.run(
            args,
            input=query,
            capture_output=True,
            text=True,
            timeout=120,
            env=env,
        )
    except subprocess.TimeoutExpired:
        return {"error": "timeout"}
    except Exception as e:
        return {"error": str(e)}

    if result.returncode != 0:
        return {"error": f"exit code {result.returncode}: {result.stderr[:200]}"}

    # Parse stdout: query_length \t before \t after \t diff
    parts = result.stdout.strip().split("\t")
    jemalloc_diff = 0
    if len(parts) == 4:
        jemalloc_diff = int(parts[3])

    # Parse stderr to find symbolized file paths
    sym_before = ""
    sym_after = ""
    for line in result.stderr.split("\n"):
        if line.startswith("Symbolized before: "):
            sym_before = line.split(": ", 1)[1].strip()
        elif line.startswith("Symbolized after: "):
            sym_after = line.split(": ", 1)[1].strip()

    # Parse symbolized heap profiles and compute diff
    heap_diff = 0
    stack_diffs = []
    if sym_before and sym_after:
        stacks_before = parse_symbolized_heap(sym_before)
        stacks_after = parse_symbolized_heap(sym_after)
        heap_diff, stack_diffs = compute_diff(stacks_before, stacks_after)

    return {
        "jemalloc_diff": jemalloc_diff,
        "heap_diff": heap_diff,
        "stack_diffs": stack_diffs,
        "error": None,
    }


def load_queries(queries_file: str) -> list:
    """Load queries from file, skip empty lines and comments."""
    with open(queries_file, "r") as f:
        return [
            line.strip()
            for line in f
            if line.strip() and not line.startswith("#")
        ]


def main():
    stop_watch = Utils.Stopwatch()
    results = []

    pr_profiler = f"{TEMP_DIR}/parser_memory_profiler"
    master_profiler = f"{TEMP_DIR}/parser_memory_profiler_master"
    profiles_dir = f"{TEMP_DIR}/profiles"

    # Check PR binary exists (downloaded by praktika from artifact)
    if not Path(pr_profiler).exists():
        results.append(
            Result(
                name="Check PR binary",
                status=Result.Status.FAILED,
                info=f"PR binary not found at {pr_profiler}",
            )
        )
        Result.create_from(results=results, stopwatch=stop_watch).complete_job()
        return

    Shell.check(f"chmod +x {pr_profiler}")
    results.append(Result(name="Check PR binary", status=Result.Status.SUCCESS))

    # Download master binary
    if download_master_binary(master_profiler):
        results.append(
            Result(name="Download master binary", status=Result.Status.SUCCESS)
        )
    else:
        results.append(
            Result(
                name="Download master binary",
                status=Result.Status.FAILED,
                info="Failed to download master binary from S3",
            )
        )
        Result.create_from(results=results, stopwatch=stop_watch).complete_job()
        return

    # Load queries
    queries = load_queries(QUERIES_FILE)
    print(f"Loaded {len(queries)} queries from {QUERIES_FILE}")

    # Create profiles directory
    os.makedirs(profiles_dir, exist_ok=True)

    # Run profilers and collect per-query results
    query_results = []
    total_regressions = 0
    total_improvements = 0
    total_master_bytes = 0
    total_pr_bytes = 0

    for i, query in enumerate(queries):
        query_num = i + 1
        query_display = query[:60].replace("\n", " ").replace("\t", " ")

        # Run master profiler
        master_prefix = f"{profiles_dir}/q{query_num}_master_"
        master_data = run_profiler_with_heap(
            master_profiler, query, master_prefix, symbolize=True
        )

        # Run PR profiler
        pr_prefix = f"{profiles_dir}/q{query_num}_pr_"
        pr_data = run_profiler_with_heap(
            pr_profiler, query, pr_prefix, symbolize=True
        )

        if master_data.get("error") or pr_data.get("error"):
            error_info = f"master: {master_data.get('error', 'ok')}, pr: {pr_data.get('error', 'ok')}"
            query_results.append(
                Result(
                    name=f"Query {query_num}: {query_display}",
                    status=Result.Status.ERROR,
                    info=error_info,
                )
            )
            continue

        # Use heap profile bytes as source of truth
        master_bytes = master_data["heap_diff"]
        pr_bytes = pr_data["heap_diff"]
        change = pr_bytes - master_bytes

        total_master_bytes += master_bytes
        total_pr_bytes += pr_bytes

        # Determine status
        if change >= CHANGE_THRESHOLD_BYTES:
            status = "FAIL"
            total_regressions += 1
        elif change <= -CHANGE_THRESHOLD_BYTES:
            status = "OK"  # improvement is OK
            total_improvements += 1
        else:
            status = "OK"

        # Build info string with stack diffs
        info_lines = [
            f"AST allocation diff (heap profile): master={master_bytes:,} bytes, PR={pr_bytes:,} bytes, change={change:+,} bytes"
        ]

        if change >= CHANGE_THRESHOLD_BYTES:
            info_lines.append(f"\nRegression: +{change:,} bytes")
        elif change <= -CHANGE_THRESHOLD_BYTES:
            info_lines.append(f"\nImprovement: {change:,} bytes")

        # Add PR stack diffs (most useful for understanding allocations)
        if pr_data["stack_diffs"]:
            info_lines.append("\nPR allocation stacks:")
            for diff_bytes, stack in pr_data["stack_diffs"][:10]:
                info_lines.append(f"  {diff_bytes:+,} bytes: {stack}")

        if master_data["stack_diffs"] and abs(change) >= CHANGE_THRESHOLD_BYTES:
            info_lines.append("\nMaster allocation stacks:")
            for diff_bytes, stack in master_data["stack_diffs"][:10]:
                info_lines.append(f"  {diff_bytes:+,} bytes: {stack}")

        query_results.append(
            Result(
                name=f"Query {query_num}: {query_display}",
                status=status,
                info="\n".join(info_lines),
            )
        )

        if query_num % 10 == 0:
            print(f"  Processed {query_num}/{len(queries)} queries...")

    # Clean up heap profile files to save disk
    for f in glob.glob(f"{profiles_dir}/*.heap*"):
        try:
            os.remove(f)
        except OSError:
            pass

    # Create "Tests" sub-result for CI report
    tests_status = Result.Status.SUCCESS
    if total_regressions > 0:
        tests_status = Result.Status.FAILED

    total_change = total_pr_bytes - total_master_bytes
    tests_info = (
        f"Queries: {len(queries)}, "
        f"Master total: {total_master_bytes:,} bytes, "
        f"PR total: {total_pr_bytes:,} bytes, "
        f"Change: {total_change:+,} bytes"
    )
    if total_regressions > 0:
        tests_info += f", Regressions: {total_regressions}"
    if total_improvements > 0:
        tests_info += f", Improvements: {total_improvements}"

    tests_result = Result.create_from(
        name="Tests",
        results=query_results,
        status=tests_status,
        info=tests_info,
    )
    results.append(tests_result)

    Result.create_from(results=results, stopwatch=stop_watch).complete_job()


if __name__ == "__main__":
    main()
