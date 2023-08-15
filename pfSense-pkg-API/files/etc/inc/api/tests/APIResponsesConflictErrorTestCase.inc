<?php

namespace API\Tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use API\Responses\ConflictError;

class APIResponsesConflictErrorTestCase extends TestCase
{
    /**
     * Checks that ConflictError objects are throwable and checks that it corresponds with an HTTP 409 status code.
     */
    public function test_conflict_error_throwable() {
        $this->assert_throws_response(
            response_id: "TEST_ERROR",
            code: 409,
            callable: function () {
                throw new ConflictError(
                    message: "test error",
                    response_id: "TEST_ERROR");
            }
        );
    }
}
