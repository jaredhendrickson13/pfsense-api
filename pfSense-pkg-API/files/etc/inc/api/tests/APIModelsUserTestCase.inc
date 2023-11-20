<?php

namespace api\tests;

use API\Core\Auth;
use API\Core\Command;
use API\Core\Model;
use API\Core\TestCase;
use API\Models\User;

class APIModelsUserTestCase extends TestCase
{
    /**
     * Checks that the user's `name` cannot be reserved by the system
     */
    public function test_name_cannot_be_reserved() {
        $this->assert_throws_response(
            response_id: "USER_CANNOT_USE_RESERVED_USERNAME",
            code: 400,
            callable: function () {
                $user = new User();
                $user->validate_name("root");
            }
        );
    }

    /**
     * Ensure the `password` field is hashed during validation when there is no existing password
     * or the password has changed.
     */
    public function test_password_is_hashed() {
        # Create a new user to test with
        $user = new User(data: ["name" => "testuser", "password" => "testpasswd"]);
        $user->create();

        # Ensure the User's `password` field has been hashed and is no longer the raw password.
        $this->assert_not_equals($user->password->value, "testpasswd");

        # Ensure the password hash remains the same unless the password value is changed
        $this->assert_equals($user->password->value, $user->validate_password($user->password->value));

        # Ensure the password hash is updated when a new password is set
        $existing_hash = $user->password->value;
        $this->assert_not_equals($existing_hash, $user->validate_password("somenewpassword"));

        # Delete the User object
        $user->delete();

    }

    /**
     * Checks that the valid `priv` values are accepted and invalid `priv` values are not.
     */
    public function test_correctly_identifies_valid_privs() {
        $this->assert_throws_response(
            response_id: "USER_PRIV_NOT_FOUND",
            code: 404,
            callable: function () {
                $user = new User();
                $user->validate_priv("this privilege does not exist");
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $user = new User();
                $user->validate_priv("page-all");
            }
        );
    }

    /**
     * Checks the `nextuid` config value is updated after user creation.
     */
    public function test_next_uid_is_incremented() {
        # Read the current `nextuid`
        $original_next_uid = (int)Model::get_config("system/nextuid");

        # Create a new user and ensure it's UID matches the original next UID
        $user = new User(data: ["name" => "test", "password" => "test"]);
        $user->create();
        $this->assert_equals($original_next_uid, $user->uid->value);

        # Ensure the `nextuid` was incremented
        $new_next_uid = (int)Model::get_config("system/nextuid");
        $this->assert_equals($original_next_uid + 1, $new_next_uid);

        # Delete the user
        $user->delete();
    }

    /**
     * Checks that a user is correctly created/update/deleted in /etc/passwd after apply.
     */
    public function test_etc_passwd() {
        # Create a new user to test with
        $user = new User(data: [
            "name" => "testuser", "password" => "testpasswd", "descr" => "Test User", "authorizedkeys" => "testkeys"]
        );
        $user->create();

        # Ensure the user is correctly created in /etc/passwd
        $this->assert_is_true(
            str_contains(
                file_get_contents("/etc/passwd"),
                "{$user->name->value}:*:{$user->uid->value}:65534:{$user->descr->value}"
            )
        );

        # Ensure the user's `authorizedkeys` are set in ~/.ssh/authorized_keys
        $this->assert_is_true(
            str_contains(
                file_get_contents("/home/{$user->name->value}/.ssh/authorized_keys"),
                $user->authorizedkeys->value
            )
        );

        # Update the user
        $user->from_representation(
            data: [
                "name" => "testuserupdate",
                "password" => "testpasswdupdate",
                "descr" => "Test User Update",
                "authorizedkeys" => "testkeysupdate"
            ]
        );
        $user->update();

        # Ensure the user is correctly updated in /etc/passwd
        $this->assert_is_true(
            str_contains(
                file_get_contents("/etc/passwd"),
                "{$user->name->value}:*:{$user->uid->value}:65534:{$user->descr->value}"
            )
        );

        # Ensure the user's `authorizedkeys` are updated in ~/.ssh/authorized_keys
        $this->assert_is_true(
            str_contains(
                file_get_contents("/home/{$user->name->value}/.ssh/authorized_keys"),
                $user->authorizedkeys->value
            )
        );

        # Delete the user
        $user->delete();

        # Ensure the user is no longer in /etc/passwd
        $this->assert_is_false(
            str_contains(
                file_get_contents("/etc/passwd"),
                $user->name->value
            )
        );
    }

    /**
     * Checks that system users cannot be deleted.
     */
    public function test_system_users_cannot_be_deleted() {
        $this->assert_throws_response(
            response_id: "USER_WITH_SYSTEM_SCOPE_CANNOT_BE_DELETED",
            code: 400,
            callable: function () {
                # Obtain the `admin` User object and try to delete it
                $admin_user = new User(id: 0);
                $admin_user->delete();
            }
        );
    }


}
