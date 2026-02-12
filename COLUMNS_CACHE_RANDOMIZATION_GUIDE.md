# Columns Cache - Testing Configuration and Randomization Guide

## Test Configuration

### Server Configuration for Tests

The columns cache can be configured for testing by placing config files in `tests/config/`:

**File**: `tests/config/columns_cache.xml`

```xml
<?xml version="1.0"?>
<clickhouse>
    <!-- Columns cache configuration -->
    <columns_cache_size>536870912</columns_cache_size>  <!-- 512MB -->
    <columns_cache_size_ratio>0.5</columns_cache_size_ratio>
    <columns_cache_policy>SLRU</columns_cache_policy>

    <!-- Enable in default profile -->
    <profiles>
        <default>
            <use_columns_cache>1</use_columns_cache>
            <enable_reads_from_columns_cache>1</enable_reads_from_columns_cache>
            <enable_writes_to_columns_cache>1</enable_writes_to_columns_cache>
        </default>
    </profiles>
</clickhouse>
```

### Running Tests

```bash
# Run all columns cache tests
./clickhouse-test --testname "columns_cache"

# Run with specific configuration
./clickhouse-test --testname "03919" --server-config=columns_cache

# Run with random settings (excluding cache to avoid conflicts)
./clickhouse-test --testname "03919" --random-settings --no-random-merge-tree-settings
```

## Randomization Support

### Settings Available for Randomization

The following columns cache settings can be randomized during testing:

| Setting | Type | Default | Random Values |
|---------|------|---------|---------------|
| `use_columns_cache` | Bool | 0 | {0, 1} |
| `enable_reads_from_columns_cache` | Bool | 1 | {0, 1} |
| `enable_writes_to_columns_cache` | Bool | 1 | {0, 1} |

### Adding to clickhouse-test Randomization

To enable randomization of columns cache settings in `clickhouse-test`, add the following to the random settings pool:

**File**: `tests/clickhouse-test` (Python script)

```python
# In the get_random_settings() function, add:

columns_cache_settings = {
    'use_columns_cache': lambda: random.choice([0, 1]),
    'enable_reads_from_columns_cache': lambda: random.choice([0, 1]),
    'enable_writes_to_columns_cache': lambda: random.choice([0, 1]),
}

# Merge with existing random_settings dict
random_settings.update(columns_cache_settings)
```

### Test Tags to Control Randomization

Tests that explicitly test columns cache functionality should disable random settings:

```sql
-- Tags: no-random-settings, no-random-merge-tree-settings

-- This ensures consistent cache behavior in these specific tests
SET use_columns_cache = 1;
SET enable_reads_from_columns_cache = 1;
SET enable_writes_to_columns_cache = 1;
```

Tests that should work with OR without cache can allow randomization:

```sql
-- No special tags needed - cache will be randomly enabled/disabled

-- Query works correctly with cache on or off
SELECT COUNT(*) FROM table;
```

## Test Matrix

### Recommended Test Coverage

| Scenario | Cache Settings | Test Purpose |
|----------|---------------|--------------|
| Correctness tests | Fixed (cache=1) | Verify cache works correctly |
| Edge case tests | Fixed (cache=1) | Test boundary conditions |
| Performance tests | Fixed (cache=1) | Measure cache benefit |
| General SQL tests | Random | Ensure cache doesn't break queries |
| Stress tests | Random | Test stability under varied conditions |

### Example Test Configurations

**1. Dedicated Cache Test** (no randomization):
```sql
-- Tags: no-random-settings, no-random-merge-tree-settings
SET use_columns_cache = 1;
SET enable_reads_from_columns_cache = 1;
SET enable_writes_to_columns_cache = 1;

-- Test cache-specific functionality
SYSTEM DROP COLUMNS CACHE;
SELECT * FROM table;  -- Populate cache
SELECT * FROM table;  -- Read from cache
```

**2. Compatibility Test** (with randomization):
```sql
-- No tags - allow randomization

-- This query should work with cache on or off
SELECT COUNT(*) FROM table WHERE condition;
```

**3. Mixed Test** (partial randomization):
```sql
-- Tags: no-random-merge-tree-settings
-- Allow random-settings but not merge-tree settings

-- Test cache with various other random settings
SELECT * FROM table;
```

## Continuous Integration

### CI Pipeline Configuration

Add columns cache tests to CI:

```yaml
# .github/workflows/test.yml or CI configuration

- name: Run Columns Cache Tests
  run: |
    ./clickhouse-test --testname "columns_cache" \\
      --server-config=columns_cache \\
      --verbose

- name: Run General Tests with Random Cache Settings
  run: |
    ./clickhouse-test --random-settings \\
      --exclude-tag=no-random-settings \\
      --iterations=3
```

### Stress Testing

For stress testing, run tests multiple times with randomized settings:

```bash
# Run each test 10 times with random settings
for i in {1..10}; do
  ./clickhouse-test --testname "03919" \\
    --random-settings \\
    --seed=$RANDOM
done
```

## Monitoring and Metrics

### ProfileEvents to Monitor

During testing, monitor these metrics:

```sql
SELECT
    event,
    value
FROM system.events
WHERE event LIKE 'ColumnsCache%'
ORDER BY event;
```

Expected events:
- `ColumnsCacheHits` - Number of cache hits
- `ColumnsCacheMisses` - Number of cache misses
- `ColumnsCacheEvictedBytes` - Bytes evicted from cache
- `ColumnsCacheEvictedEntries` - Number of entries evicted

### Performance Baselines

Establish baselines for cache effectiveness:

```sql
-- Run with cache disabled
SET use_columns_cache = 0;
SELECT ... FROM large_table;  -- Measure time T1

-- Run with cache enabled (cold)
SYSTEM DROP COLUMNS CACHE;
SET use_columns_cache = 1;
SELECT ... FROM large_table;  -- Measure time T2 (≈ T1)

-- Run with cache enabled (warm)
SELECT ... FROM large_table;  -- Measure time T3 (< T1)

-- Cache effectiveness = (T1 - T3) / T1 * 100%
```

## Debugging Failed Tests

### Common Issues

1. **Cache not enabled**: Check settings in query
2. **Cache size too small**: Increase `columns_cache_size`
3. **Data not cached**: Ensure `enable_writes_to_columns_cache = 1`
4. **Cache evicted**: Monitor `ColumnsCacheEvictedEntries`

### Debug Queries

```sql
-- Check if cache is configured
SELECT * FROM system.server_settings
WHERE name LIKE 'columns_cache%';

-- Check current query settings
SELECT
    getSetting('use_columns_cache'),
    getSetting('enable_reads_from_columns_cache'),
    getSetting('enable_writes_to_columns_cache');

-- View cache statistics
SELECT * FROM system.events
WHERE event LIKE 'ColumnsCache%';
```

## Best Practices

1. **Test Isolation**: Use `SYSTEM DROP COLUMNS CACHE` between test cases
2. **Deterministic Data**: Use `FROM numbers(N)` for reproducible results
3. **No Time-Dependent**: Avoid `now()` in test assertions
4. **Clear Tags**: Mark tests with appropriate `no-random-*` tags
5. **Reference Files**: Keep reference files updated after changes

## Summary

- **Configuration**: Use `tests/config/columns_cache.xml` for test setup
- **Randomization**: Add cache settings to `clickhouse-test` random pool
- **Tags**: Use `no-random-settings` for cache-specific tests
- **Monitoring**: Track `ColumnsCacheHits` and related events
- **Coverage**: Mix fixed and random settings across test suite

This ensures comprehensive testing of the columns cache feature under both controlled and randomized conditions.
