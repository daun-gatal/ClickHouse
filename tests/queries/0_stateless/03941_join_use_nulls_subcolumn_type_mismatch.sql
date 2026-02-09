-- Regression test for "Unexpected return type from getSubcolumn. Expected UInt64. Got Nullable(UInt64)" with join_use_nulls
-- https://github.com/ClickHouse/ClickHouse/issues/...

SET allow_experimental_statistics=1;
SET join_use_nulls = true;

-- Create test database and tables
CREATE DATABASE IF NOT EXISTS test_d9;
CREATE VIEW test_d9.v315 AS (SELECT 30::Int256 AS c0 WHERE 1);

CREATE DATABASE IF NOT EXISTS test_d1;
CREATE TABLE test_d1.t0 (c0 Map(Date,Date), c1 Int32 NULL) ENGINE = Log();

CREATE DATABASE IF NOT EXISTS test_d9_2;
CREATE TABLE test_d9_2.v28 (c0 LowCardinality(String), c1 Int64) Engine=MergeTree() ORDER BY tuple();

CREATE DATABASE IF NOT EXISTS test_d8;
CREATE DICTIONARY test_d8.d302 (c0 Date DEFAULT -19 INJECTIVE) PRIMARY KEY (c0) SOURCE(CLICKHOUSE(DB 'test_d9_2' TABLE 'v28' QUERY 'SELECT 1 as c0')) LAYOUT(CACHE(SIZE_IN_CELLS 9742717)) LIFETIME(60);

CREATE DATABASE IF NOT EXISTS test_d12;
CREATE DICTIONARY test_d12.d308 (c0 String DEFAULT '\\'man\\'' INJECTIVE) PRIMARY KEY (c0) SOURCE(CLICKHOUSE(DB 'test_d8' TABLE 'd302' QUERY 'SELECT 1 as c0')) LAYOUT(COMPLEX_KEY_HASHED_ARRAY()) LIFETIME(MIN 10 MAX 30);

-- This query should not throw "Unexpected return type" error
-- The issue is that t1d0.c0 is from the right side of a FULL JOIN, so it becomes Nullable with join_use_nulls=true
-- When accessing the subcolumn .size, the getSubcolumn function must also return Nullable(UInt64)
SELECT t0d0.c0, 9::Int128, t0d0.c0, t1d0.`c0.size`, t0d0.c0 
FROM test_d9.v315 AS t0d0 
FINAL FULL JOIN test_d12.d308 AS t1d0 
ON t0d0.c0 = t1d0.`c0.size` 
LIMIT 1 FORMAT Null;

-- Cleanup
DROP DATABASE test_d12;
DROP DATABASE test_d8;
DROP DATABASE test_d9_2;
DROP DATABASE test_d1;
DROP DATABASE test_d9;
