<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use function RESTAPI\Core\Tools\bandwidth_to_bits;
use function RESTAPI\Core\Tools\is_assoc_array;
use function RESTAPI\Core\Tools\to_upper_camel_case;

class APICoreToolsTestCase extends TestCase {
    /**
     * Checks that the is_assoc_array() function correctly identifies associative arrays from indexed arrays.
     */
    public function test_is_assoc_array(): void {
        $this->assert_is_true(is_assoc_array(['test' => 'this is an associative array']));
        $this->assert_is_false(is_assoc_array([0 => 'this is not an associative array']));
    }

    /**
     * Checks that the bandwidth_to_bits() function correctly converts a provided bandwidth and scale into the
     * total number of bits of the bandwidth.
     */
    public function test_bandwidth_to_bits(): void {
        # Set scale factors we can use to convert various bandwidth values into total bit count
        $gigabit_scale_factor = 1024 * 1024 * 1024;
        $megabit_scale_factor = 1024 * 1024;
        $kilobit_scale_factor = 1024;

        $this->assert_equals(bandwidth_to_bits(500, 'b'), 500);
        $this->assert_equals(bandwidth_to_bits(800, 'Kb'), 800 * $kilobit_scale_factor);
        $this->assert_equals(bandwidth_to_bits(500, 'Mb'), 500 * $megabit_scale_factor);
        $this->assert_equals(bandwidth_to_bits(10, 'Gb'), 10 * $gigabit_scale_factor);

        # Ensure an error is thrown if the scale is not a valid option
        $this->assert_throws_response(
            response_id: 'TOOLS_BANDWIDTH_TO_BITS_INVALID_SCALE_OPTION',
            code: 500,
            callable: function () {
                bandwidth_to_bits(500, 'invalid!');
            },
        );
    }

    /**
     * Checks that the to_upper_camel_case() function correctly converts a provided string into upper camel case.
     */
    public function test_to_upper_camel_case(): void {
        $this->assert_equals(to_upper_camel_case('this_is_a_test'), 'ThisIsATest');
        $this->assert_equals(to_upper_camel_case('this-is-a-test'), 'ThisIsATest');
        $this->assert_equals(to_upper_camel_case('this is a test'), 'ThisIsATest');
        $this->assert_equals(to_upper_camel_case('_this is-a test'), 'ThisIsATest');
    }
}
