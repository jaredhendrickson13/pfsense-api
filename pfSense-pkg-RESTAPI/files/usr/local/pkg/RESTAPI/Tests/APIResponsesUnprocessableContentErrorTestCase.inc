<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\UnprocessableContentError;

class APIResponsesUnprocessableContentErrorTestCase extends TestCase {
    /**
     * Checks that FailedDependencyError objects are throwable and checks that it corresponds with an HTTP 422 status code.
     */
    public function test_unprocessable_content_error_throwable(): void {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 422,
            callable: function () {
                throw new UnprocessableContentError(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }
}
