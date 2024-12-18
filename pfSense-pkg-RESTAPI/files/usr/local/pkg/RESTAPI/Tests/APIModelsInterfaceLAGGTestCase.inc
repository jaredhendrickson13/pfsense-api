<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\InterfaceLAGG;

class APIModelsInterfaceLAGGTestCase extends TestCase {
    /**
     * Checks that the `members` field must be an existing real interface.
     */
    public function test_member_field_must_be_real_interface(): void {
        # Ensure an error is thrown if the member interface does not exist
        $this->assert_throws_response(
            response_id: 'INTERFACE_LAGG_MEMBER_DOES_NOT_EXIST',
            code: 400,
            callable: function () {
                $lagg = new InterfaceLAGG();
                $lagg->validate_members('nonexistent');
            },
        );

        # Ensure no error is thrown if the member interface exists
        $this->assert_does_not_throw(
            callable: function () {
                $lagg = new InterfaceLAGG();
                $lagg->validate_members($this->env['PFREST_OPT1_IF']);
            },
        );
    }

    /**
     * Checks that the `failovermaster` field must be a member of the LAGG.
     */
    public function test_failovermaster_field_must_be_member_of_lagg(): void {
        # Ensure an error is thrown if the failover master is not a member
        $this->assert_throws_response(
            response_id: 'INTERFACE_LAGG_MASTER_NOT_MEMBER',
            code: 400,
            callable: function () {
                $lagg = new InterfaceLAGG();
                $lagg->members->value = [$this->env['PFREST_OPT1_IF']];
                $lagg->validate_failovermaster('nonexistent');
            },
        );

        # Ensure no error is thrown if the failover master is a member
        $this->assert_does_not_throw(
            callable: function () {
                $lagg = new InterfaceLAGG();
                $lagg->members->value = [$this->env['PFREST_OPT1_IF']];
                $lagg->validate_failovermaster($this->env['PFREST_OPT1_IF']);
            },
        );
    }

    /**
     * Ensure that we can create, read, update and delete a LAGG interface.
     */
    public function test_crud(): void {
        # Create a new LAGG interface
        $lagg = new InterfaceLAGG(members: [$this->env['PFREST_OPT1_IF']], proto: 'lacp');
        $lagg->create();
        $this->assert_is_not_empty($lagg->laggif->value);
        $ifconfig_output = new Command("ifconfig {$lagg->laggif->value}");
        $this->assert_str_contains($ifconfig_output->output, 'laggproto lacp');
        $this->assert_str_contains($ifconfig_output->output, "laggport: {$this->env['PFREST_OPT1_IF']}");

        # Update the LAGG interface
        $lagg->proto->value = 'failover';
        $lagg->update();
        $ifconfig_output = new Command("ifconfig {$lagg->laggif->value}");
        $this->assert_str_contains($ifconfig_output->output, 'laggproto failover');
        $this->assert_str_does_not_contain($ifconfig_output->output, 'laggproto lacp');

        # Delete the LAGG interface
        $lagg->delete();
        $ifconfig_output = new Command("ifconfig {$lagg->laggif->value}");
        $this->assert_str_contains($ifconfig_output->output, 'does not exist');
    }

    /**
     * Ensure that we cannot delete a LAGG that is in use.
     */
    public function test_cannot_delete_lagg_in_use(): void {
        # Create a new LAGG
        $lagg = new InterfaceLAGG(members: [$this->env['PFREST_OPT1_IF']], proto: 'lacp');
        $lagg->create();

        # Mock an interface that is using the LAGG
        InterfaceLAGG::set_config('interfaces/opt99', ['if' => $lagg->laggif->value]);

        # Ensure we cannot delete the LAGG
        $this->assert_throws_response(
            response_id: 'INTERFACE_LAGG_CANNOT_BE_DELETED_WHILE_IN_USE',
            code: 409,
            callable: function () use ($lagg) {
                # Try to delete the LAGG
                $lagg->delete();
            },
        );

        # Remove the mock interface and actually delete the LAGG
        InterfaceLAGG::del_config('interfaces/opt99');
        $lagg->delete();
    }
}
