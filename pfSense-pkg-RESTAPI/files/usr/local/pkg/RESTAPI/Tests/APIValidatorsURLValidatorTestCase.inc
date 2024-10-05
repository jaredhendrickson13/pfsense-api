<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\URLValidator;

class APIValidatorsURLValidatorTestCase extends TestCase {
    /**
     * Ensure the URLValidator does not throw an exception when given a valid URL is given, but does throw an exception
     * when given an invalid URL.
     */
    public function test_url_validator(): void {
        # Ensure valid URLs are accepted
        $validator = new URLValidator();
        $validator->validate('http://example.com');

        # Ensure allow keywords are accepted
        $validator = new URLValidator(allow_keywords: ['not a url!']);
        $validator->validate('not a url!');

        # Ensure invalid URLs are rejected
        $this->assert_throws_response(
            response_id: 'URL_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $validator = new URLValidator();
                $validator->validate('not a url!');
            },
        );
    }
}
