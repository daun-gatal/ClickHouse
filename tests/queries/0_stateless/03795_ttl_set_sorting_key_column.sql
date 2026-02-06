-- Test that TTL GROUP BY SET clamping prevents sort order violation
-- when SET modifies columns used in the sorting key expression

DROP TABLE IF EXISTS t_ttl_sort_clamp;

CREATE TABLE t_ttl_sort_clamp
(
    `timestamp` DateTime,
    `id` String,
    `value` UInt16
)
ENGINE = MergeTree
ORDER BY (id, toStartOfDay(timestamp))
TTL timestamp + toIntervalDay(1)
    GROUP BY id, toStartOfDay(timestamp)
    SET timestamp = max(timestamp) + toIntervalYear(100),
        id = argMax(id, timestamp),
        value = max(value);

INSERT INTO t_ttl_sort_clamp VALUES (parseDateTimeBestEffort('2000-06-09 10:00'), 'pepe', 1000);
INSERT INTO t_ttl_sort_clamp VALUES (parseDateTimeBestEffort('2000-06-10 10:00'), 'pepe', 1000);
INSERT INTO t_ttl_sort_clamp VALUES (parseDateTimeBestEffort('2000-06-10 11:00'), 'pepe', 2000);
INSERT INTO t_ttl_sort_clamp VALUES (parseDateTimeBestEffort('2000-06-10 12:00'), 'pepe', 1200);
INSERT INTO t_ttl_sort_clamp VALUES (parseDateTimeBestEffort('2000-06-10 13:00'), 'pepe', 1300);

OPTIMIZE TABLE t_ttl_sort_clamp FINAL;

-- Should return results without sort order violation
-- Values should be clamped so sorting key is preserved
SELECT id, value FROM t_ttl_sort_clamp ORDER BY id, toStartOfDay(timestamp);

DROP TABLE t_ttl_sort_clamp;
