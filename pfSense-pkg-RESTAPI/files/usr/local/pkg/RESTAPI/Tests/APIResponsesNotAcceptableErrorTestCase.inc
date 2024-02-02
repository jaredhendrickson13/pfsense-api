<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\NotAcceptableError;

class APIResponsesNotAcceptableErrorTestCase extends TestCase
{
    /**
     * Checks that NotAcceptableError objects are throwable and checks that it corresponds with an HTTP 406 status code.
     */
    public function test_not_found_error_throwable()
    {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 406,
            callable: function () {
                throw new NotAcceptableError(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }
}
