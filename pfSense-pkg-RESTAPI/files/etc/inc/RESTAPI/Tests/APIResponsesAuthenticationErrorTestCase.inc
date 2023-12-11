<?php

namespace RESTAPI\Tests;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\AuthenticationError;

class APIResponsesAuthenticationErrorTestCase extends TestCase
{
    /**
     * Checks that AuthenticationError objects are throwable and checks that it corresponds with an HTTP
     * 401 status code.
     */
    public function test_authentication_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 401,
            callable: function () {
                throw new AuthenticationError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
