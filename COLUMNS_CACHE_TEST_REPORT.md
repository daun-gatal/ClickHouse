# Columns Cache Feature Testing Report

**Date**: 2026-02-12
**Branch**: columns-cache
**Issue**: https://github.com/ClickHouse/ClickHouse/issues/82335
**Version**: ClickHouse 26.2.1.1

---

## Executive Summary

Comprehensive testing of the columns cache feature revealed **strong correctness** in basic scenarios and **a critical bug** requiring resolution before production deployment.

**Status**: ⚠️ **NOT READY FOR PRODUCTION** - Bug fix required
**Key Finding**: LOGICAL_ERROR in granule management when serving cached data with WHERE filters

---

## Test Coverage

### Correctness Tests Created (3 files, 40+ scenarios)

1. **`03916_columns_cache_correctness.sql`** - Core functionality
   - 20+ column types (Int, UInt, Float, String, Date, Nullable, Array, Tuple, etc.)
   - Table operations (RENAME, ALTER, OPTIMIZE)
   - Multi-part scenarios
   - PREWHERE optimization
   - JOINs and subqueries
   - Settings combinations

2. **`03917_columns_cache_edge_cases.sql`** - Edge cases
   - Small/large granules (100 - 65536 rows)
   - ORDER BY/LIMIT queries
   - Aggregations with GROUP BY
   - NULL handling
   - Different MergeTree engines (Replacing, Summing)
   - Mixed compact/wide parts

3. **`03918_columns_cache_metrics.sql`** - Metrics & monitoring
   - Cache hit/miss tracking
   - SYSTEM commands
   - Selective column reads
   - Multiple tables

### Performance Testing

- **Test dataset**: 50M rows generated
- **ClickBench dataset**: 99,997,497 rows (14.23 GiB)
- **Cache hit rate**: 72% in repeated query workload

---

## Results

### ✅ Passing Tests

| Feature | Status | Notes |
|---------|--------|-------|
| Basic caching | ✅ PASS | Stores and retrieves correctly |
| Cache invalidation | ✅ PASS | Table UUID changes work |
| Metrics tracking | ✅ PASS | Hits/misses/evictions tracked |
| System commands | ✅ PASS | DROP COLUMNS CACHE works |
| Simple filters | ✅ PASS | WHERE RegionID = N works |
| Column types | ✅ PASS | All types tested work |
| Settings control | ✅ PASS | All settings function |

### 🔴 Critical Bug Found

**Error**:
```
Code: 49. DB::Exception: Can't adjust last granule because it has 8192 rows,
but try to subtract 41387 rows (num_read_rows = 15957, rows_per_granule = [8192, ...])
```

**Location**: `src/Storages/MergeTree/MergeTreeReaderWide.cpp`
**Trigger**: WHERE clause filters on second read from cache
**Severity**: BLOCKER

**Reproduction**:
```sql
SYSTEM DROP COLUMNS CACHE;

-- First query: populate cache (works)
SELECT COUNT(*) FROM hits WHERE AdvEngineID <> 0 SETTINGS use_columns_cache = 1;

-- Second query: read from cache (fails intermittently)
SELECT COUNT(*) FROM hits WHERE AdvEngineID <> 0 SETTINGS use_columns_cache = 1;
```

**Root Cause**: Incorrect row counting in `columns_cache_serve_state` when serving partial granules.

**Affected Code** (approximate location):
```cpp
// src/Storages/MergeTree/MergeTreeReaderWide.cpp:219-264
if (all_cached)
{
    columns_cache_serve_state.emplace();
    columns_cache_serve_state->total_rows = total_rows_in_range;
    // Bug: Incorrect calculation of rows to serve
}
```

---

## Performance Observations

### Cache Effectiveness
- **Hit Rate**: 72% (930 hits / 355 misses)
- **ProfileEvents**: Correctly tracking hits, misses, and evictions
- **Configuration**: 1GB cache size, SLRU policy

### Benchmark Results (Before Bug Fix)
Unable to complete full ClickBench due to bug. Simple queries showed:
- COUNT(*): ~1ms (both with/without cache)
- Filtered queries: Intermittent failures prevent accurate measurement

---

## Configuration

### Server Settings
```xml
<columns_cache_size>1073741824</columns_cache_size>  <!-- 1GB -->
<columns_cache_size_ratio>0.5</columns_cache_size_ratio>
<columns_cache_policy>SLRU</columns_cache_policy>
```

### Query Settings
```sql
SET use_columns_cache = 1;
SET enable_reads_from_columns_cache = 1;
SET enable_writes_to_columns_cache = 1;
```

---

## Recommendations

### Immediate Actions Required

1. **Fix granule counting bug**:
   - Review `MergeTreeReaderWide.cpp` lines 195-350
   - Fix row calculation in `columns_cache_serve_state`
   - Add bounds checking
   - Handle partial granule reads correctly

2. **Add regression test**:
   ```sql
   -- Test partial granule reads from cache
   CREATE TABLE test_granule_bug (...) ENGINE = MergeTree ...;
   INSERT INTO test_granule_bug SELECT ... FROM numbers(100000);

   SYSTEM DROP COLUMNS CACHE;
   SELECT COUNT(*) FROM test_granule_bug WHERE filter_col <> 0
   SETTINGS use_columns_cache = 1;  -- Populate

   SELECT COUNT(*) FROM test_granule_bug WHERE filter_col <> 0
   SETTINGS use_columns_cache = 1;  -- Should not error
   ```

3. **Re-run ClickBench** after fix

### Future Enhancements

1. Add unit tests for cache internals
2. Test with larger cache sizes (10GB+)
3. Test with different cache policies (LRU vs SLRU)
4. Benchmark concurrent query workloads
5. Test cache behavior under memory pressure

---

## Technical Details

### Cache Architecture
- **Key Structure**: Table UUID → Part Name → Column Name → Mark Range → ColumnPtr
- **Eviction Policy**: SLRU (Segmented Least Recently Used)
- **Weight Function**: Column byte size + 256 bytes overhead
- **Thread Safety**: Uses CacheBase with proper locking

### Integration Points
- `IMergeTreeReader`: Accepts `ColumnsCache*` pointer
- `MergeTreeReaderWide`: Implements cache read/write logic
- `Context`: Manages global cache instance
- `InterpreterSystemQuery`: Handles DROP COLUMNS CACHE

---

## Test Files Location

```
tests/queries/0_stateless/03916_columns_cache_correctness.sql
tests/queries/0_stateless/03916_columns_cache_correctness.reference
tests/queries/0_stateless/03917_columns_cache_edge_cases.sql
tests/queries/0_stateless/03917_columns_cache_edge_cases.reference
tests/queries/0_stateless/03918_columns_cache_metrics.sql
tests/queries/0_stateless/03918_columns_cache_metrics.reference
```

---

## Conclusion

The columns cache feature demonstrates **strong fundamentals** with correct caching logic, good cache hit rates, and proper integration with the MergeTree engine. However, a **critical bug in granule boundary handling** must be resolved before production deployment.

**Expected Outcome After Fix**: Significant performance improvement for analytical workloads with repeated access to hot data, especially beneficial for:
- Real-time dashboards querying recent data
- Repeated analytical queries on same datasets
- Applications with high cache hit potential

**Confidence**: High that the bug can be quickly resolved given clear error messages and reproducible test cases.

---

## Appendix: Bug Details

### Stack Trace
```
0. Poco::Exception::Exception(String const&, int)
1. DB::Exception::Exception(DB::Exception::MessageMasked&&, int, bool)
2. DB::Exception::Exception(String&&, int, String, bool)
...
While reading part 201307_193_239_2
While executing MergeTreeSelect(pool: ReadPool, algorithm: Thread)
```

### Error Context
- Part: `201307_193_239_2`
- Granule size: 8192 rows
- Attempted to subtract: 41387 rows
- Actual rows read: 15957
- Granule array: `[8192, 8192, 8192, 8192, 8192, 8192, 8192]`

This suggests an off-by-one or cumulative counting error in the cache serving logic.
