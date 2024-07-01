<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Responses\MediaTypeError;

class APIResponsesMediaTypeErrorTestCase extends TestCase {
    /**
     * Checks that MediaTypeError objects are throwable and checks that it corresponds with an HTTP 415 status code.
     */
    public function test_media_type_error_throwable() {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 415,
            callable: function () {
                throw new MediaTypeError(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }
}
