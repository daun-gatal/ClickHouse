-- Tags: zookeeper

-- Test for chained RENAME COLUMN mutations applied to a part that was attached
-- with old column names. When two renames like c0->c3->c4 are applied together
-- to a part that has column c0, the rename chain must be properly resolved.
-- Without the fix, the mutated part would end up with empty columns, causing
-- a LOGICAL_ERROR exception in getTotalColumnsSize.

SET mutations_sync = 0;

DROP TABLE IF EXISTS test_chained_rename;

CREATE TABLE test_chained_rename (c0 String)
ENGINE = ReplicatedMergeTree('/clickhouse/tables/{database}/test_chained_rename', 'r1')
ORDER BY tuple()
PARTITION BY tuple()
SETTINGS min_bytes_for_wide_part = 1;

INSERT INTO test_chained_rename VALUES ('hello');

-- Detach the partition so the part goes to detached/
ALTER TABLE test_chained_rename DETACH PARTITION tuple();

-- Rename c0 -> c3 -> c4 while the table has no parts.
-- Both mutations complete trivially (no parts to mutate).
ALTER TABLE test_chained_rename RENAME COLUMN c0 TO c3;
ALTER TABLE test_chained_rename RENAME COLUMN c3 TO c4;

-- Attach the partition back. The part has column c0, but the table now has c4.
-- Both renames need to be applied together to this part.
ALTER TABLE test_chained_rename ATTACH PARTITION tuple();

-- Wait for mutations to be applied to the attached part.
SYSTEM SYNC REPLICA test_chained_rename;

SELECT * FROM test_chained_rename;

DROP TABLE test_chained_rename;
