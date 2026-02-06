#!/usr/bin/env python3
"""
Parser Memory Profiler CI Check

Compares parser memory allocations between the PR build and master build.
Downloads master binary from S3 and compares allocation patterns.
"""

import html
import subprocess
from pathlib import Path

from ci.praktika.result import Result
from ci.praktika.utils import Shell, Utils

TEMP_DIR = f"{Utils.cwd()}/ci/tmp"
QUERIES_FILE = f"{Utils.cwd()}/utils/parser-memory-profiler/test_queries.txt"
MASTER_PROFILER_URL = "https://clickhouse-builds.s3.us-east-1.amazonaws.com/master/aarch64/parser_memory_profiler"


def download_master_binary(dest_path: str) -> bool:
    """Download master parser_memory_profiler from S3."""
    print(f"Downloading master binary from {MASTER_PROFILER_URL}")
    return Shell.check(f"wget -nv -O {dest_path} {MASTER_PROFILER_URL} && chmod +x {dest_path}")


def run_profiler(binary_path: str, queries_file: str, output_file: str) -> bool:
    """Run parser_memory_profiler on queries and save results."""
    results = []
    
    with open(queries_file, "r") as f:
        queries = [line.strip() for line in f if line.strip() and not line.startswith("#")]
    
    print(f"Running profiler on {len(queries)} queries...")
    
    for i, query in enumerate(queries):
        try:
            result = subprocess.run(
                [binary_path],
                input=query,
                capture_output=True,
                text=True,
                timeout=30
            )
            if result.returncode == 0:
                # Output format: query_length \t before \t after \t diff
                parts = result.stdout.strip().split("\t")
                if len(parts) == 4:
                    results.append({
                        "query_num": i + 1,
                        "query": query[:100],  # Truncate for display
                        "length": int(parts[0]),
                        "before": int(parts[1]),
                        "after": int(parts[2]),
                        "diff": int(parts[3])
                    })
        except subprocess.TimeoutExpired:
            print(f"Query {i+1} timed out")
        except Exception as e:
            print(f"Query {i+1} failed: {e}")
    
    # Save results
    with open(output_file, "w") as f:
        f.write("query_num\tquery\tlength\tbefore\tafter\tdiff\n")
        for r in results:
            f.write(f"{r['query_num']}\t{r['query']}\t{r['length']}\t{r['before']}\t{r['after']}\t{r['diff']}\n")
    
    return len(results) > 0


def generate_report(pr_results_file: str, master_results_file: str, report_file: str) -> dict:
    """Generate comparison report between PR and master results."""
    
    def load_results(filepath):
        results = {}
        with open(filepath, "r") as f:
            next(f)  # Skip header
            for line in f:
                parts = line.strip().split("\t")
                if len(parts) >= 6:
                    query_num = int(parts[0])
                    results[query_num] = {
                        "query": parts[1],
                        "diff": int(parts[5])
                    }
        return results
    
    pr_results = load_results(pr_results_file)
    master_results = load_results(master_results_file)
    
    # Compare results
    comparisons = []
    total_pr = 0
    total_master = 0
    regressions = 0
    improvements = 0
    
    for query_num in sorted(set(pr_results.keys()) | set(master_results.keys())):
        pr_diff = pr_results.get(query_num, {}).get("diff", 0)
        master_diff = master_results.get(query_num, {}).get("diff", 0)
        query = pr_results.get(query_num, master_results.get(query_num, {})).get("query", "")
        
        change = pr_diff - master_diff
        change_pct = (change / master_diff * 100) if master_diff > 0 else 0
        
        total_pr += pr_diff
        total_master += master_diff
        
        status = "same"
        if change > 1000:  # More than 1KB increase
            status = "regression"
            regressions += 1
        elif change < -1000:  # More than 1KB decrease
            status = "improvement"
            improvements += 1
        
        comparisons.append({
            "query_num": query_num,
            "query": query,
            "pr_diff": pr_diff,
            "master_diff": master_diff,
            "change": change,
            "change_pct": change_pct,
            "status": status
        })
    
    # Generate HTML report
    html_content = """<!DOCTYPE html>
<html>
<head>
    <title>Parser Memory Profiler Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #333; }
        .summary { background: #f5f5f5; padding: 15px; border-radius: 5px; margin-bottom: 20px; }
        .regression { background: #ffcccc; }
        .improvement { background: #ccffcc; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background: #4CAF50; color: white; }
        tr:nth-child(even) { background: #f2f2f2; }
        .query { font-family: monospace; font-size: 12px; max-width: 400px; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
    </style>
</head>
<body>
    <h1>Parser Memory Profiler Report</h1>
    <div class="summary">
        <h2>Summary</h2>
        <p><strong>Total PR allocations:</strong> {total_pr:,} bytes</p>
        <p><strong>Total Master allocations:</strong> {total_master:,} bytes</p>
        <p><strong>Total change:</strong> {total_change:+,} bytes ({total_change_pct:+.1f}%)</p>
        <p><strong>Regressions (>1KB):</strong> {regressions}</p>
        <p><strong>Improvements (>1KB):</strong> {improvements}</p>
    </div>
    <h2>Query Details</h2>
    <table>
        <tr>
            <th>#</th>
            <th>Query</th>
            <th>PR (bytes)</th>
            <th>Master (bytes)</th>
            <th>Change</th>
            <th>Status</th>
        </tr>
""".format(
        total_pr=total_pr,
        total_master=total_master,
        total_change=total_pr - total_master,
        total_change_pct=((total_pr - total_master) / total_master * 100) if total_master > 0 else 0,
        regressions=regressions,
        improvements=improvements
    )

    for c in comparisons:
        row_class = ""
        if c["status"] == "regression":
            row_class = "regression"
        elif c["status"] == "improvement":
            row_class = "improvement"
        
        query_escaped = html.escape(c['query'])
        html_content += f"""        <tr class="{row_class}">
            <td>{c['query_num']}</td>
            <td class="query" title="{query_escaped}">{query_escaped}</td>
            <td>{c['pr_diff']:,}</td>
            <td>{c['master_diff']:,}</td>
            <td>{c['change']:+,} ({c['change_pct']:+.1f}%)</td>
            <td>{c['status']}</td>
        </tr>
"""

    html_content += """    </table>
</body>
</html>
"""

    with open(report_file, "w") as f:
        f.write(html_content)
    
    return {
        "total_pr": total_pr,
        "total_master": total_master,
        "regressions": regressions,
        "improvements": improvements
    }


def main():
    stop_watch = Utils.Stopwatch()
    results = []

    # Paths
    pr_profiler = f"{TEMP_DIR}/parser_memory_profiler"
    master_profiler = f"{TEMP_DIR}/parser_memory_profiler_master"
    pr_results_file = f"{TEMP_DIR}/pr_results.tsv"
    master_results_file = f"{TEMP_DIR}/master_results.tsv"
    report_file = f"{TEMP_DIR}/parser_memory_report.html"
    
    # Check PR binary exists (downloaded by praktika from artifact)
    if not Path(pr_profiler).exists():
        results.append(Result(
            name="Check PR binary",
            status=Result.Status.FAILED,
            info=f"PR binary not found at {pr_profiler}"
        ))
        Result.create_from(results=results, stopwatch=stop_watch).complete_job()
        return
    
    Shell.check(f"chmod +x {pr_profiler}")
    results.append(Result(name="Check PR binary", status=Result.Status.SUCCESS))
    
    # Download master binary
    if download_master_binary(master_profiler):
        results.append(Result(name="Download master binary", status=Result.Status.SUCCESS))
    else:
        results.append(Result(
            name="Download master binary",
            status=Result.Status.FAILED,
            info="Failed to download master binary from S3"
        ))
        Result.create_from(results=results, stopwatch=stop_watch).complete_job()
        return
    
    # Run PR profiler
    if run_profiler(pr_profiler, QUERIES_FILE, pr_results_file):
        results.append(Result(name="Run PR profiler", status=Result.Status.SUCCESS))
    else:
        results.append(Result(name="Run PR profiler", status=Result.Status.FAILED))
    
    # Run master profiler
    if run_profiler(master_profiler, QUERIES_FILE, master_results_file):
        results.append(Result(name="Run master profiler", status=Result.Status.SUCCESS))
    else:
        results.append(Result(name="Run master profiler", status=Result.Status.FAILED))
    
    # Generate comparison report
    files = []
    try:
        summary = generate_report(pr_results_file, master_results_file, report_file)
        files.append(report_file)
        
        # Determine overall status
        status = Result.Status.SUCCESS
        info_msg = f"PR: {summary['total_pr']:,} bytes, Master: {summary['total_master']:,} bytes"
        
        if summary["regressions"] > 0:
            info_msg += f", {summary['regressions']} regressions"
        if summary["improvements"] > 0:
            info_msg += f", {summary['improvements']} improvements"
        
        results.append(Result(
            name="Generate report",
            status=status,
            info=info_msg
        ))
    except Exception as e:
        results.append(Result(
            name="Generate report",
            status=Result.Status.FAILED,
            info=str(e)
        ))
    
    Result.create_from(
        results=results,
        stopwatch=stop_watch,
        files=files
    ).complete_job()


if __name__ == "__main__":
    main()
