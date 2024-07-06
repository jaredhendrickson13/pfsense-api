<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\NotFoundError;

class APIResponsesNotFoundErrorTestCase extends TestCase {
    /**
     * Checks that NotFoundError objects are throwable and checks that it corresponds with an HTTP 404 status code.
     */
    public function test_not_found_error_throwable() {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 404,
            callable: function () {
                throw new NotFoundError(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }
}
