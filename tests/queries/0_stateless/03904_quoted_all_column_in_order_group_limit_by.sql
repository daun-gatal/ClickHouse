-- Quoted identifier `all` should be treated as a column reference,
-- not as the special ALL keyword in ORDER BY, GROUP BY, and LIMIT BY.

-- ORDER BY `all` as a column reference (quoted)
SELECT -number AS `all` FROM numbers(5) ORDER BY `all`;

-- ORDER BY ALL expands to all SELECT columns (unquoted)
SELECT number FROM numbers(5) ORDER BY ALL;

-- ORDER BY all (lowercase, unquoted) also expands
SELECT number FROM numbers(5) ORDER BY all;

-- ORDER BY `all` DESC
SELECT -number AS `all` FROM numbers(5) ORDER BY `all` DESC;

-- GROUP BY `all` as a column reference (quoted)
SELECT -number AS `all`, count() FROM numbers(5) GROUP BY `all` ORDER BY `all`;

-- GROUP BY ALL expands to all SELECT columns (unquoted)
SELECT number, count() FROM numbers(5) GROUP BY ALL ORDER BY number;

-- LIMIT BY `all` as a column reference (quoted)
SELECT -number AS `all` FROM numbers(10) ORDER BY `all` LIMIT 1 BY `all`;

-- LIMIT BY ALL expands to all columns (unquoted)
SELECT -number AS `all` FROM numbers(10) ORDER BY `all` LIMIT 1 BY ALL;

-- ORDER BY `all` with enable_order_by_all = 0 should still work as a column reference
SELECT -number AS `all` FROM numbers(5) ORDER BY `all` SETTINGS enable_order_by_all = 0;

-- ORDER BY ALL with enable_order_by_all = 0 treats ALL as a column name, but no such column exists
SELECT -number AS `all` FROM numbers(5) ORDER BY ALL SETTINGS enable_order_by_all = 0; -- { serverError UNKNOWN_IDENTIFIER }
