<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\InterfaceVLAN;

class APIModelsInterfaceVLANTestCase extends TestCase {
    /**
     * Checks that created VLANs are correctly configuring by checking ifconfig.
     */
    public function test_vlan_configured_in_ifconfig() {
        # Create a new VLAN interface
        $test_vlan = new InterfaceVLAN(
            data: [
                "if" => "em1",
                "tag" => 2,
                "pcp" => 4
            ]
        );
        $test_vlan->create();

        # Ensure the newly created VLAN is correctly configuring by checking ifconfig
        $ifconfig_output = shell_exec("ifconfig em1.2");
        $this->assert_str_contains($ifconfig_output, "vlan: 2");
        $this->assert_str_contains($ifconfig_output, "vlanpcp: 4");

        # Update the VLAN interface to change the tag and pcp values
        $test_vlan->tag->value = 55;
        $test_vlan->pcp->value = 7;
        $test_vlan->update();

        # Ensure the old VLAN is no longer present
        $ifconfig_output = shell_exec("ifconfig em1.2");
        $this->assert_equals($ifconfig_output, null);

        # Ensure the new VLAN is present
        $ifconfig_output = shell_exec("ifconfig em1.55");
        $this->assert_str_contains($ifconfig_output, "vlan: 55");
        $this->assert_str_contains($ifconfig_output, "vlanpcp: 7");

        # Delete the VLAN interface
        $test_vlan->delete();

        # Ensure the VLAN is no longer present
        $ifconfig_output = shell_exec("ifconfig em1.55");
        $this->assert_equals($ifconfig_output, null);
    }

    /**
     * Checks to ensure a VLAN's `if` and `tag` must be unique together
     */
    public function test_vlan_if_and_tag_unique_requirement() {
        # Create a new VLAN interface
        $test_vlan = new InterfaceVLAN(
            data: [
                "if" => "em1",
                "tag" => 5
            ]
        );
        $test_vlan->create();

        # Ensure attempts to create another VLAN with the same `if` and `tag` throw an error
        $this->assert_throws_response(
            response_id: "MODEL_FIELDS_NOT_UNIQUE_TOGETHER",
            code: 400,
            callable: function() {
                # Create a new VLAN interface
                $bad_vlan = new InterfaceVLAN(
                    data: [
                        "if" => "em1",
                        "tag" => 5
                    ]
                );
                $bad_vlan->validate();
            }
        );

        # Delete the created VLAN used for testing
        $test_vlan->delete();
    }

    /**
     * Checks that the VLANs requested parent interface must be known to the system
     */
    public function test_vlan_parent_exists_requirement() {
        # Ensure attempts to create a VLAN for a parent interface that doesn't exist fail
        $this->assert_throws_response(
            response_id: "VLAN_INTERFACE_PARENT_NOT_FOUND",
            code: 400,
            callable: function() {
                # Create a new VLAN interface
                $bad_vlan = new InterfaceVLAN(
                    data: [
                        "if" => "not an known interface",
                        "tag" => 5
                    ]
                );
                $bad_vlan->validate();
            }
        );
    }

    # TODO: Add test to ensure VLAN cannot be added if in use by QinQ once a QinQ Model has been developed
}
