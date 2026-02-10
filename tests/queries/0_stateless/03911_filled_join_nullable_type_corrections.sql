-- Tags: no-random-settings

-- Regression test for "Unexpected return type from equals. Expected Nullable(UInt8). Got UInt8."
-- This error occurred when nullable type corrections were not applied to filled joins.
-- The fix ensures type corrections are applied for ALL filled joins, not just StorageJoin.
--
-- Related issues:
-- https://github.com/ClickHouse/ClickHouse/issues/96101
-- https://github.com/ClickHouse/ClickHouse/issues/95678
-- https://github.com/ClickHouse/ClickHouse/issues/89802

SET enable_analyzer = 1;
SET query_plan_use_new_logical_join_step = 0;

DROP TABLE IF EXISTS t1;
DROP TABLE IF EXISTS t2_join;

CREATE TABLE t1 (x Nullable(UInt32), str String) ENGINE = Memory;
CREATE TABLE t2_join (x UInt32, s String) ENGINE = Join(ALL, RIGHT, x);

INSERT INTO t1 VALUES (1, 'a'), (2, 'b'), (NULL, 'c');
INSERT INTO t2_join VALUES (1, 'A'), (3, 'C');

-- Test RIGHT JOIN with QUALIFY (main bug scenario)
SELECT 'RIGHT JOIN with QUALIFY:';
SELECT *
FROM t1 RIGHT JOIN t2_join USING (x)
QUALIFY x = 1;

-- Test RIGHT JOIN with WHERE (similar comparison in WHERE clause)
SELECT 'RIGHT JOIN with WHERE:';
SELECT *
FROM t1 RIGHT JOIN t2_join USING (x)
WHERE x = 1;

-- Test RIGHT JOIN without filter (ensure nullable conversion is correct)
SELECT 'RIGHT JOIN without filter:';
SELECT x, str, s
FROM t1 RIGHT JOIN t2_join USING (x)
ORDER BY x;

DROP TABLE t1;
DROP TABLE t2_join;

-- Test LEFT JOIN scenario (symmetric case)
DROP TABLE IF EXISTS t3;
DROP TABLE IF EXISTS t4_join;

CREATE TABLE t3 (x UInt32, str String) ENGINE = Memory;
CREATE TABLE t4_join (x Nullable(UInt32), s String) ENGINE = Join(ALL, LEFT, x);

INSERT INTO t3 VALUES (1, 'a'), (2, 'b'), (3, 'c');
INSERT INTO t4_join VALUES (1, 'A'), (NULL, 'NULL'), (3, 'C');

SELECT 'LEFT JOIN with QUALIFY:';
SELECT *
FROM t3 LEFT JOIN t4_join USING (x)
QUALIFY x = 1;

SELECT 'LEFT JOIN with WHERE:';
SELECT *
FROM t3 LEFT JOIN t4_join USING (x)
WHERE x = 1;

DROP TABLE t3;
DROP TABLE t4_join;

-- Test FULL JOIN scenario
DROP TABLE IF EXISTS t5;
DROP TABLE IF EXISTS t6_join;

CREATE TABLE t5 (x Nullable(UInt32), str String) ENGINE = Memory;
CREATE TABLE t6_join (x Nullable(UInt32), s String) ENGINE = Join(ALL, FULL, x);

INSERT INTO t5 VALUES (1, 'a'), (2, 'b'), (NULL, 'c');
INSERT INTO t6_join VALUES (1, 'A'), (4, 'D'), (NULL, 'NULL');

SELECT 'FULL JOIN with QUALIFY:';
SELECT *
FROM t5 FULL JOIN t6_join USING (x)
QUALIFY x = 1;

SELECT 'FULL JOIN with WHERE:';
SELECT *
FROM t5 FULL JOIN t6_join USING (x)
WHERE x = 1
ORDER BY str, s;

DROP TABLE t5;
DROP TABLE t6_join;
