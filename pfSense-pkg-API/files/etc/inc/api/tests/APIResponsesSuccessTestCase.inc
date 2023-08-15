<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\Success;

class APIResponsesSuccessTestCase extends TestCase
{
    /**
     * Checks that Success objects are throwable and checks that it corresponds with an HTTP 200 status code.
     */
    public function test_success_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_SUCCESS",
            code: 200,
            callable: function () {
                throw new Success(
                    message: "test success",
                    response_id: "TEST_SUCCESS");
            }
        );
    }
}
