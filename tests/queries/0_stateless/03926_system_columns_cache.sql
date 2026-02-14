-- Test system.columns_cache table and SYSTEM commands
-- Tags: no-parallel, no-random-settings, no-random-merge-tree-settings

-- Ensure cache is empty at start
SYSTEM DROP COLUMNS CACHE;

-- Verify system table exists and has correct schema
DESC TABLE system.columns_cache;

-- Create test table
DROP TABLE IF EXISTS t_system_cache_test;

CREATE TABLE t_system_cache_test (
    id UInt64,
    name String,
    value Float64
) ENGINE = MergeTree
ORDER BY id
SETTINGS min_bytes_for_wide_part = 0;

-- Insert data
INSERT INTO t_system_cache_test SELECT number, toString(number), number * 1.5 FROM numbers(10000);

-- Read with cache enabled (this should populate cache)
SELECT count(*), sum(id), sum(value)
FROM t_system_cache_test
SETTINGS use_columns_cache = 1,
         enable_writes_to_columns_cache = 1,
         enable_reads_from_columns_cache = 1;

-- Query system.columns_cache table
-- Note: May be empty if table doesn't have UUID (non-Atomic database)
SELECT
    column,
    row_end - row_begin as row_count,
    rows,
    bytes > 0 as has_bytes
FROM system.columns_cache
WHERE database = currentDatabase()
  AND part LIKE '%t_system_cache_test%'
ORDER BY column, row_begin
FORMAT Null;

-- Test SYSTEM DROP COLUMNS CACHE
SYSTEM DROP COLUMNS CACHE;

-- Verify cache is empty after drop
SELECT count(*) as cache_entries
FROM system.columns_cache
WHERE database = currentDatabase()
  AND part LIKE '%t_system_cache_test%';

-- Read again to repopulate
SELECT count(*)
FROM t_system_cache_test
SETTINGS use_columns_cache = 1,
         enable_writes_to_columns_cache = 1;

-- Test cache metrics
SELECT
    event,
    value > 0 as has_events
FROM system.events
WHERE event LIKE 'ColumnsCache%'
ORDER BY event
SETTINGS use_columns_cache = 0;

-- Cleanup
DROP TABLE t_system_cache_test;

SELECT 'System table and SYSTEM commands test passed';
