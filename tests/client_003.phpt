--TEST--
[CLIENT] Execute commands with binary data (binary-safe)

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

if (!@is_readable('/dev/urandom')) {
    die('skip Cannot read from /dev/urandom"');
}

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$rlite = create_rlite_connection(RLITE_MEMORY);

$resource = fopen('/dev/urandom', 'r');
$data = fread($resource, 1024);
fclose($resource);

rlite_command($rlite, ['DEL', 'test']);
rlite_command($rlite, ['SET', 'test', $data]);
$response = rlite_command($rlite, ['GET', 'test']);

var_dump($response === $data);

--EXPECT--
bool(true)
