<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\ServerError;

class APIResponsesServerErrorTestCase extends TestCase
{
    /**
     * Checks that ServerError objects are throwable and checks that it corresponds with an HTTP 500 status code.
     */
    public function test_server_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 500,
            callable: function () {
                throw new ServerError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
