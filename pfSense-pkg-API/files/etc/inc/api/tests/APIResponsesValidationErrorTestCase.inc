<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\ValidationError;

class APIResponsesValidationErrorTestCase extends TestCase
{
    /**
     * Checks that ValidationError objects are throwable and checks that it corresponds with an HTTP 400 status code.
     */
    public function test_validation_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 400,
            callable: function () {
                throw new ValidationError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
