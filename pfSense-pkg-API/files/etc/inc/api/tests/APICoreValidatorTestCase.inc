<?php

namespace API\Tests;

use API;

class APICoreValidatorTestCase extends API\Core\TestCase {
    /**
     * Check that the default Validator class's 'validate()' method throws an exception indicating it hasn't been
     * setup/extended by a child class.
     */
    public function test_default_validate_method() {
        $this->assert_throws_response(
            ref_id: "VALIDATOR_INCOMPLETE",
            callable: function () {
                (new API\Core\Validator())->validate("test");
            }
        );
    }

}