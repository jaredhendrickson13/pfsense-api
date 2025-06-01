<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FreeRADIUSUser;

class APIModelsFreeRADIUSUserTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-freeradius3'];

    /**
     * Checks that we can create, read and delete FreeRADIUSUser models.
     */
    public function test_crud(): void {
        # Create a new FreeRADIUSUser with password and ensure it is present in the database
        $user = new FreeRADIUSUser(
            username: 'testuser',
            password: 'testpassword',
            password_encryption: 'Cleartext-Password',
            motp_enable: false,
            description: 'Test User',
        );
        $user->create();
        $raddb = file_get_contents('/usr/local/etc/raddb/users');
        $this->assert_str_contains($raddb, 'testuser" Cleartext-Password := "testpassword"');

        # Ensure we can read the created user from the config
        $read_user = new FreeRADIUSUser(id: $user->id);
        $this->assert_equals($read_user->username->value, 'testuser');
        $this->assert_equals($read_user->password_encryption->value, 'Cleartext-Password');
        $this->assert_equals($read_user->motp_enable->value, false);
        $this->assert_equals($read_user->description->value, 'Test User');

        # Ensure we can update the user
        $user = new FreeRADIUSUser(
            id: $read_user->id,
            username: 'motptestuser',
            motp_enable: true,
            motp_authmethod: 'motp',
            motp_secret: 'abcdef0123456789',
            motp_pin: '1234',
            motp_offset: 30,
        );
        $user->update();
        $raddb = file_get_contents('/usr/local/etc/raddb/users');
        $this->assert_str_does_not_contain($raddb, 'testuser" Cleartext-Password := "testpassword"');
        $this->assert_str_contains($raddb, '"motptestuser" Auth-Type = motp');
        $this->assert_str_contains($raddb, 'MOTP-Init-Secret = abcdef0123456789');
        $this->assert_str_contains($raddb, 'MOTP-PIN = 1234');
        $this->assert_str_contains($raddb, 'MOTP-Offset = 30');

        # Delete the user and ensure it is removed from the database
        $user->delete();
        $raddb = file_get_contents('/usr/local/etc/raddb/users');
        $this->assert_str_does_not_contain($raddb, '"motptestuser" Auth-Type = motp');
    }
}
