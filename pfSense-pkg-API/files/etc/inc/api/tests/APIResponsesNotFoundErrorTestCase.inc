<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\NotFoundError;

class APIResponsesNotFoundErrorTestCase extends TestCase
{
    /**
     * Checks that NotFoundError objects are throwable and checks that it corresponds with an HTTP 404 status code.
     */
    public function test_not_found_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 404,
            callable: function () {
                throw new NotFoundError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
