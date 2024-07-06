<?php

namespace RESTAPI\Tests;

use RESTAPI;

/**
 * Defines test case for testing RESTAPI\Core\Validator.
 */
class APICoreValidatorTestCase extends RESTAPI\Core\TestCase {
    /**
     * Check that the default Validator class's 'validate()' method throws an exception indicating it hasn't been
     * setup/extended by a child class.
     */
    public function test_default_validate_method() {
        $this->assert_throws_response(
            response_id: 'VALIDATOR_INCOMPLETE',
            code: 500,
            callable: function () {
                (new RESTAPI\Core\Validator())->validate('test');
            },
        );
    }
}
