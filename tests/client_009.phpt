--TEST--
[CLIENT] Persistence

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_PATH);

rlite_command($rlite, ['SET', 'key', 'value']);

$rlite = create_rlite_connection(RLITE_PATH);

var_dump(rlite_command($rlite, ['GET', 'key']));

@unlink(RLITE_PATH);

--EXPECT--
string(5) "value"
