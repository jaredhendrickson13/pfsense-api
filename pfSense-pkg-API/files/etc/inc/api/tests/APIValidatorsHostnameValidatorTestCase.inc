<?php

namespace API\Tests;

use API\Core\TestCase;
use api\validators\HostnameValidator;

class APIValidatorsHostnameValidatorTestCase extends TestCase
{
    /**
     * Checks that the HostnameValidator accepts valid hostnames, and rejects non hostname values
     */
    public function test_validate() {
        $this->assert_throws_response(
            response_id: "HOSTNAME_VALIDATOR_FAILED",
            code: 400,
            callable: function () {
                $validator = new HostnameValidator();
                $validator->validate("!!! NOT A HOSTNAME !!!");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $validator = new HostnameValidator();
                $validator->validate("hostname");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $validator = new HostnameValidator();
                $validator->validate("also-a-hostname");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $validator = new HostnameValidator();
                $validator->validate("also.a.hostname");
            }
        );
    }
}
