<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\ForbiddenError;

class APIResponsesForbiddenErrorTestCase extends TestCase
{
    /**
     * Checks that ForbiddenError objects are throwable and checks that it corresponds with an HTTP 403 status code.
     */
    public function test_forbidden_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 403,
            callable: function () {
                throw new ForbiddenError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
