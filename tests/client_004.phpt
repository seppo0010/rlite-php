--TEST--
[CLIENT] Integer response

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

var_dump(rlite_command($rlite, ['RPUSH', 'test', '1', '2', '3']));

--EXPECT--
int(3)
