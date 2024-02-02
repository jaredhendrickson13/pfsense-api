<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\FailedDependencyError;

class APIResponsesFailedDependencyErrorTestCase extends TestCase
{
    /**
     * Checks that FailedDependencyError objects are throwable and checks that it corresponds with an HTTP 424 status code.
     */
    public function test_failed_dependency_error_throwable(): void
    {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 424,
            callable: function () {
                throw new FailedDependencyError(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }
}
