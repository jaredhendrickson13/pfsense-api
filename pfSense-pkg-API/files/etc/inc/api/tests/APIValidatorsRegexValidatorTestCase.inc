<?php

namespace api\tests;

require_once("api/auto_loader.inc");

use API\Core\TestCase;
use api\validators\RegexValidator;

class APIValidatorsRegexValidatorTestCase extends TestCase
{
    /**
     * Checks that the RegexValidator exceeds when the value matches the patterns, and fails when it doesn't.
     */
    public function test_regex_validator() {
        # Ensure an error is thrown if the context is not inverted and the value does not match the pattern
        $this->assert_throws_response(
            response_id: "REGEX_VALIDATOR_FAILED",
            code: 400,
            callable: function() {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: false);
                $regex_validator->validate("!@# Does not match pattern! #@!");
            }
        );

        # Ensure an error is not thrown if the context is not inverted and the value does match the pattern
        $this->assert_does_not_throw(
            callable: function() {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: false);
                $regex_validator->validate("matchespattern");
            }
        );

        # Ensure an error is thrown if the context is inverted and the value does match the pattern
        $this->assert_throws_response(
            response_id: "REGEX_VALIDATOR_FAILED",
            code: 400,
            callable: function() {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: true);
                $regex_validator->validate("matchespattern");
            }
        );

        # Ensure an error is not thrown if the context is inverted and the value does not match the pattern
        $this->assert_does_not_throw(
            callable: function() {
                $regex_validator = new RegexValidator(pattern: "/^[a-zA-Z0-9\.\-_]+$/", invert: true);
                $regex_validator->validate("!@# Does not match pattern! #@!");
            }
        );
    }
}
