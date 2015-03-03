--TEST--
[CLIENT] Execute commands, string response

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

rlite_command($rlite, ['DEL', 'test']);
rlite_command($rlite, ['SET', 'test', '1']);
$response = rlite_command($rlite, ['GET', 'test']);

var_dump($response);

--EXPECT--
string(1) "1"
