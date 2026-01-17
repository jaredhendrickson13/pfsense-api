<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\UserGroup;

class APIModelsUserGroupTestCase extends TestCase {
    /**
     * Ensure the GID increases with each new Model.
     */
    public function test_gid_increment() {
        # Create UserGroup model to test with
        $user_group = new UserGroup();

        # Ensure the default GID assigned to this object Model is the system/nextgid value
        $this->assert_equals($user_group->gid->default, (int) $user_group->get_config('system/nextgid', 0));
    }

    /**
     * Checks that the maximum length of the `name` field is effectively limited to 16 characters for `local`
     * UserGroups, but is allowed up to 64 characters for `remote` UserGroups.
     */
    public function test_name_maximum_length() {
        # Create a UserGroup to test with
        $user_group = new UserGroup();

        # Ensure an error is thrown if a `local` group has a name greater than 16 characters
        $this->assert_throws_response(
            response_id: 'USER_GROUP_NAME_EXCEEDS_MAXIMUM_LENGTH_FOR_LOCAL_SCOPE',
            code: 400,
            callable: function () use ($user_group) {
                $user_group->scope->value = 'local';
                $user_group->validate_name('this_name_is_too_long_for_a_local_group');
            },
        );

        # Ensure no error is thrown if a `remote` group has a name greater than 16 but less than 64 characters
        $this->assert_does_not_throw(
            callable: function () use ($user_group) {
                $user_group->scope->value = 'remote';
                $user_group->validate_name('this_name_is_too_long_for_a_local_group');
            },
        );
    }

    /**
     * Checks that any value assigned to the `priv` field must be a valid pfSense privilege name.
     */
    public function test_validate_priv() {
        # Create a UserGroup to test with
        $user_group = new UserGroup();

        # Ensure an error is thrown if a `local` group has a name greater than 16 characters
        $this->assert_throws_response(
            response_id: 'USER_GROUP_PRIV_NOT_FOUND',
            code: 404,
            callable: function () use ($user_group) {
                $user_group->validate_priv('not a real priv name');
            },
        );
    }

    /**
     * Checks that user groups are correctly configured on the OS when created, updated, and deleted.
     */
    public function test_crud() {
        # Create a UserGroup to test with
        $user_group = new UserGroup(data: ['name' => 'testgroup', 'scope' => 'local', 'member' => ['admin']]);
        $user_group->create();

        # Ensure the created user group is present in /etc/group
        $this->assert_str_contains(
            file_get_contents('/etc/group'),
            "{$user_group->name->value}:*:{$user_group->gid->value}:root",
        );

        # Update the user group and ensure it was updated in /etc/group
        $user_group->name->value = 'updatedtestgroup';
        $user_group->member->value = [];
        $user_group->update();

        # Ensure the previous user group is no longer present in /etc/group
        $this->assert_str_does_not_contain(
            file_get_contents('/etc/group'),
            "{$user_group->name->value}:*:{$user_group->gid->value}:root",
        );

        # Ensure the updated user group is in /etc/group
        $this->assert_str_contains(
            file_get_contents('/etc/group'),
            "{$user_group->name->value}:*:{$user_group->gid->value}:",
        );

        # Delete the user group and ensure it is no longer present in /etc/group
        $user_group->delete();
        $this->assert_str_does_not_contain(file_get_contents('/etc/group'), $user_group->name->value);
    }

    /**
     * Checks that groups with the `system` scope cannot be deleted or replaced.
     */
    public function test_cannot_delete_or_replace_system_scope_user_groups() {
        # Create a UserGroup to test with
        $user_group = new UserGroup();

        # Replace all existing user groups with an empty set
        $user_group->replace_all(data: []);

        # Loop through all existing groups and ensure the groups with `scope` set to `system` are still present
        foreach ($user_group->read_all()->model_objects as $model_object) {
            $this->assert_equals($model_object->scope->value, 'system');
        }

        # Ensure attempts to delete UserGroups with the system scope are rejected
        $this->assert_throws_response(
            response_id: 'MODEL_CANNOT_DELETE_PROTECTED_OBJECT',
            code: 403,
            callable: function () {
                # Create a UserGroup to test with
                $user_group = new UserGroup(id: 0);
                $user_group->delete();
            },
        );
    }

    /**
     * Ensures that UserGroups cannot be created or updated with the 'system' scope unless the existing object also has
     * the 'system' scope.
     */
    public function test_cannot_create_or_update_user_groups_with_system_scope(): void {
        # Ensure attempts to create UserGroups with the system scope are rejected
        $this->assert_throws_response(
            response_id: 'USER_GROUP_SCOPE_CANNOT_BE_SET_TO_SYSTEM',
            code: 422,
            callable: function () {
                $user_group = new UserGroup(name: 'testgroup', scope: 'system');
                $user_group->create();
            },
        );

        # Ensure attempts to update existing UserGroups to have the system scope are rejected
        $this->assert_throws_response(
            response_id: 'USER_GROUP_SCOPE_CANNOT_BE_SET_TO_SYSTEM',
            code: 422,
            callable: function () {
                # Create a local UserGroup to test with
                $user_group = new UserGroup(data: ['name' => 'testgroup', 'scope' => 'local']);
                $user_group->create();

                # Attempt to update the scope to `system`
                $user_group->scope->value = 'system';
                $user_group->update();
            },
        );

        # Ensure we CAN update an existing UserGroup with the system scope to still have the system scope
        $this->assert_does_not_throw(
            callable: function () {
                # Create a UserGroup with the system scope to test with
                $admin_group = UserGroup::query(name: 'admins')->first();
                $admin_group->validate_scope('system');
            },
        );
    }
}
