#!/usr/bin/env bash
# Tags: no-fasttest
# Regression test for https://github.com/ClickHouse/ClickHouse/issues/75708
# Reading an ORC file where one column is entirely NULL should not cause
# LOGICAL_ERROR about invalid number of rows in Chunk.

CURDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=../shell_config.sh
. "$CURDIR"/../shell_config.sh

DATA_DIR=${CLICKHOUSE_TMP}/$CLICKHOUSE_TEST_UNIQUE_NAME
mkdir -p "$DATA_DIR"

# Generate an ORC file with an all-NULL Nullable(String) column
$CLICKHOUSE_LOCAL -q "
    SELECT
        number AS id,
        CAST(NULL AS Nullable(String)) AS null_col,
        toString(number) AS value
    FROM numbers(50)
    FORMAT ORC
" > "$DATA_DIR/all_null.orc"

# SELECT * should work without LOGICAL_ERROR
$CLICKHOUSE_LOCAL -q "SELECT count() FROM file('$DATA_DIR/all_null.orc', ORC)"

# Selecting the NULL column explicitly should also work
$CLICKHOUSE_LOCAL -q "SELECT countIf(null_col IS NULL) FROM file('$DATA_DIR/all_null.orc', ORC)"

# Selecting non-NULL columns should work
$CLICKHOUSE_LOCAL -q "SELECT count(), min(id), max(id) FROM file('$DATA_DIR/all_null.orc', ORC)"

rm -rf "$DATA_DIR"
