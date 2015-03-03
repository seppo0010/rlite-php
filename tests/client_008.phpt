--TEST--
[CLIENT] Array response

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

rlite_command($rlite, ['RPUSH', 'test', '1', '2', '3']);

var_dump(rlite_command($rlite, ['LRANGE', 'test', '0', '-1']));

--EXPECT--
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
