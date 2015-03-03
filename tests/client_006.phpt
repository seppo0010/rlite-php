--TEST--
[CLIENT] Error response

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

var_dump(rlite_command($rlite, ['SET', 'test']));

--EXPECT--
Warning: rlite_command(): wrong number of arguments for 'set' command in /Users/seppo/Projects/free-software/mine/rlite-php/tests/client_006.php on line 6
bool(false)
