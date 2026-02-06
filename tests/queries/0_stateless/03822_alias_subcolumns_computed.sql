SET enable_analyzer=1;

CREATE TABLE tuple_subcolumn (
  c Int,
  t Tuple(a Int, b String) ALIAS (1, 'abc')
) Engine=Log;
INSERT INTO TABLE tuple_subcolumn (c) VALUES (1);
SELECT t.a, t.b FROM tuple_subcolumn;

CREATE TABLE array_subcolumn (
    c Int,
    arr Array(Int) ALIAS [1, 2]
) Engine=Log;
INSERT INTO TABLE array_subcolumn (c) VALUES (1);
SELECT arr.size0 FROM array_subcolumn;
EXPLAIN actions=1 SELECT arr.size0 FROM array_subcolumn;

SET flatten_nested=false;
CREATE TABLE nested_subcolumn (
    c Int,
    n Nested(a Int8, b String) ALIAS [(1, 'a'), (2, 'b')]
) Engine=Memory;
INSERT INTO TABLE nested_subcolumn (c) VALUES (1);
SELECT n.size0, n.a, n.b FROM nested_subcolumn;
EXPLAIN actions=1 SELECT n.size0, n.a, n.b FROM nested_subcolumn;
