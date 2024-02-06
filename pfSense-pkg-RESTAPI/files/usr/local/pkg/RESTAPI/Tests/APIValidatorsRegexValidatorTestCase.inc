<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\RegexValidator;

class APIValidatorsRegexValidatorTestCase extends TestCase {
    /**
     * Checks that the RegexValidator exceeds when the value matches the patterns, and fails when it doesn't.
     */
    public function test_regex_validator() {
        # Ensure an error is thrown if the context is not inverted and the value does not match the pattern
        $this->assert_throws_response(
            response_id: 'REGEX_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: false);
                $regex_validator->validate('!@# Does not match pattern! #@!');
            },
        );

        # Ensure an error is not thrown if the context is not inverted and the value does match the pattern
        $this->assert_does_not_throw(
            callable: function () {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: false);
                $regex_validator->validate('matchespattern');
            },
        );

        # Ensure an error is thrown if the context is inverted and the value does match the pattern
        $this->assert_throws_response(
            response_id: 'REGEX_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: true);
                $regex_validator->validate('matchespattern');
            },
        );

        # Ensure an error is not thrown if the context is inverted and the value does not match the pattern
        $this->assert_does_not_throw(
            callable: function () {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: true);
                $regex_validator->validate('!@# Does not match pattern! #@!');
            },
        );

        # Ensure an error is not thrown if the value is an allowed keyword
        $this->assert_does_not_throw(
            callable: function () {
                $regex_validator = new RegexValidator(
                    pattern: "/^[a-zA-Z0-9\.\-_]+$/",
                    invert: false,
                    allowed_keywords: ['!@#test_value#@!'],
                );
                $regex_validator->validate('!@#test_value#@!');
            },
        );

        # Ensure an error is not thrown if the value is an allowed keyword, even if the context is inverted
        $this->assert_does_not_throw(
            callable: function () {
                $regex_validator = new RegexValidator(
                    pattern: "/^[a-zA-Z0-9\.\-_]+$/",
                    invert: true,
                    allowed_keywords: ['test_value'],
                );
                $regex_validator->validate('test_value');
            },
        );
    }
}
