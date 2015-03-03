--TEST--
[CLIENT] Connect to rlite

--SKIPIF--
<?php
require_once 'testsuite_skipif.inc';

--FILE--
<?php
require_once 'testsuite_utilities.inc';

$redis = create_rlite_connection(RLITE_MEMORY);
var_dump($redis);

--EXPECTF--
resource(%d) of type (rlite connection)
