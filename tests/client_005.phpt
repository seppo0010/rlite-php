--TEST--
[CLIENT] Status response

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

var_dump(rlite_command($rlite, ['SET', 'test', '1']));

--EXPECT--
string(2) "OK"
