<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\TestCase;
use RESTAPI\Validators\EmailAddressValidator;

class APIValidatorsEmailAddressValidatorTestCase extends TestCase {
    /**
     * Checks that the EmailAddressValidator succeeds when the value is an email address and fails when its not.
     */
    public function test_email_address_validator(): void {
        # Ensure an error is thrown when the value is not an email address
        $this->assert_throws_response(
            response_id: 'EMAIL_ADDRESS_VALIDATOR_FAILED',
            code: 400,
            callable: function () {
                $regex_validator = new EmailAddressValidator();
                $regex_validator->validate('Not an email address');
            },
        );

        # Ensure an error is not thrown when the value is an email address
        $this->assert_does_not_throw(
            callable: function () {
                $regex_validator = new EmailAddressValidator();
                $regex_validator->validate('validemail@example.com');
            },
        );
    }
}
