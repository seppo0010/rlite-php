<?php

require_once 'testsuite_configuration.inc';

if (!function_exists('create_rlite_connection')) {
    function create_rlite_connection($path) {
        global $connect_flags;

		$link = rlite_connect($path, 0);

        if (!$link) {
            printf("Cannot open database in path '%s'\n", $path);

            return false;
        }

        return $link;
    }
} else {
    printf("skip Eeeek/BUG/FIXME - connect.inc included twice! skipif bug?\n");
}