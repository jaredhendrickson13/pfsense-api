<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\EmailNotificationSettings;

class APIModelsEmailNotificationSettingsTestCase extends TestCase {
    /**
     * Checks if we can correctly read and update the email notification settings.
     */
    public function test_read_and_update(): void {
        $email_settings = new EmailNotificationSettings(
            disable: false,
            ipaddress: 'example.com',
            port: '465',
            timeout: 24,
            ssl: true,
            sslvalidate: false,
            fromaddress: 'from@example.com',
            notifyemailaddress: 'to@example.com',
            authentication_mechanism: 'LOGIN',
            username: 'user',
            password: 'password',
        );
        $email_settings->update();
        $this->assert_is_not_empty(EmailNotificationSettings::read_all()->to_representation());
        $this->assert_type(EmailNotificationSettings::read_all()->to_representation(), type: 'array');
    }
}
