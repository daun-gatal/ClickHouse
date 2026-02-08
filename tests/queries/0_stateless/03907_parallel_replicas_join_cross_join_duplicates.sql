-- Regression test for https://github.com/ClickHouse/ClickHouse/issues/74337
-- JOIN + CROSS JOIN with parallel replicas should not produce duplicate rows.

SET enable_parallel_replicas = 1, max_parallel_replicas = 3, cluster_for_parallel_replicas = 'test_cluster_one_shard_three_replicas_localhost', parallel_replicas_for_non_replicated_merge_tree = 1;

DROP TABLE IF EXISTS t_pr_v1;
DROP TABLE IF EXISTS t_pr_v2;

CREATE TABLE t_pr_v1 (id Int32) ENGINE = MergeTree() ORDER BY id;
CREATE TABLE t_pr_v2 (value Int32) ENGINE = MergeTree() ORDER BY value;

INSERT INTO t_pr_v1 VALUES (1);
INSERT INTO t_pr_v2 VALUES (1);

SELECT * FROM t_pr_v1 AS t1 JOIN t_pr_v1 AS t2 USING (id) CROSS JOIN t_pr_v2 AS n1;

SELECT * FROM t_pr_v1 AS t1 JOIN t_pr_v1 AS t2 USING (id) CROSS JOIN t_pr_v2 AS n1 CROSS JOIN t_pr_v2 AS n2;

DROP TABLE t_pr_v1;
DROP TABLE t_pr_v2;
