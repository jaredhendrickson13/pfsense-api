<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\FilterNameValidator;

class APIValidatorsFilterNameValidatorTestCase extends TestCase {
    /**
     * Checks that system reserved names are not allowed as filter names.
     */
    public function test_cannot_use_reserved_names() {
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_RESERVED_NAME_USED",
            code: 400,
            callable: function () {
                $test_validator = new FilterNameValidator();
                $test_validator->validate("sshguard");
            }
        );
    }

    /**
     * Checks that filter names cannot begin with `pkg_`.
     */
    public function test_cannot_start_with_pkg() {
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_CANNOT_START_WITH_PKG",
            code: 400,
            callable: function () {
                $test_validator = new FilterNameValidator();
                $test_validator->validate("pkg_test");
            }
        );
    }

    /**
     * Checks that filter names cannot be entirely numeric.
     */
    public function test_cannot_be_pfsense_interface_id() {
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_NAME_IN_USE_BY_INTERFACE",
            code: 400,
            callable: function () {
                $test_validator = new FilterNameValidator();
                $test_validator->validate("wan");
            }
        );
    }

    # TODO: Add test to ensure filter names cannot be existing ifgroup names
}
