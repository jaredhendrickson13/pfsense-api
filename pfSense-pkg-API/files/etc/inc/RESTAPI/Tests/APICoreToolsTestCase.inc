<?php

namespace RESTAPI\Tests;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\TestCase;
use function RESTAPI\Core\Tools\is_assoc_array;

class APICoreToolsTestCase extends TestCase
{
    /**
     * Checks that the is_assoc_array() function correctly identifies associative arrays from indexed arrays.
     */
    public function test_is_assoc_array() {
        $this->assert_is_true(is_assoc_array(["test" => "this is an associative array"]));
        $this->assert_is_false(is_assoc_array([0 => "this is not an associative array"]));
    }
}
