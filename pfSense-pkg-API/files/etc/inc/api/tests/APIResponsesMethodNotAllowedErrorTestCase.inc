<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\MethodNotAllowedError;
use API\Responses\ValidationError;

class APIResponsesMethodNotAllowedErrorTestCase extends TestCase
{
    /**
     * Checks that MethodNotAllowedError objects are throwable and checks that it corresponds with an HTTP 405
     * status code.
     */
    public function test_method_not_allowed_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 405,
            callable: function () {
                throw new MethodNotAllowedError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
