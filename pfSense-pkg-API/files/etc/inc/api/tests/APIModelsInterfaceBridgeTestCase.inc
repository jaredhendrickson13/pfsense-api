<?php

namespace API\Tests;

use API\Core\Command;
use API\Core\Model;
use API\Core\TestCase;
use API\Models\InterfaceBridge;

class APIModelsInterfaceBridgeTestCase extends TestCase
{
    /**
     * Checks that interface bridges cannot be created with wireless interface members unless
     * the wireless mode is set to `hostap`.
     */
    public function test_no_non_hostap_wireless_members() {
        # Mock a non-hostap wireless interface in config for testing
        Model::set_config("interfaces/lan/wireless", ["mode" =>"test"]);

        # Try to create an interface bridge with the mocked wireless interface and ensure it throws an error
        $this->assert_throws_response(
            response_id: "INTERFACE_BRIDGE_NO_NON_HOSTAP_WIRELESS_MEMBERS",
            code: 400,
            callable: function () {
                $if_bridge = new InterfaceBridge(data: ["members" => ["lan"]]);
                $if_bridge->validate();
            }
        );
    }

    /**
     * Checks that interface bridges cannot be created with members that are also assigned to an
     * existing captive portal
     */
    public function test_no_captive_portal_assigned_members() {
        # Mock a captive portal for testing
        Model::set_config("captiveportal/0", ["interface" =>"wan,lan"]);

        # Try to create an interface bridge with the if from the mocked captive portal and ensure it throws an error
        $this->assert_throws_response(
            response_id: "INTERFACE_BRIDGE_MEMBER_IS_ASSIGNED_TO_CAPTIVE_PORTAL",
            code: 400,
            callable: function () {
                $if_bridge = new InterfaceBridge(data: ["members" => ["lan"]]);
                $if_bridge->validate();
            }
        );
    }

    /**
     * Checks that the `get_next_bridgeif() method correctly obtains the next bridgeif value
     */
    public function test_get_next_bridgeif() {
        # Construct a interface bridge object to test with
        $if_bridge = new InterfaceBridge();

        # Since we have no configured bridge interfaces, the first value should be `bridge0`
        $this->assert_equals($if_bridge->get_next_bridgeif(), "bridge0");

        # Create a new bridge interface and ensure the next bridgeif is incremented
        $if_bridge->members->value = ["lan"];
        $if_bridge->create();
        $this->assert_equals($if_bridge->get_next_bridgeif(), "bridge1");

        # Delete the bridge interface and ensure the next bridgeif returns to bridge0
        $if_bridge->delete();
        $this->assert_equals($if_bridge->get_next_bridgeif(), "bridge0");
    }

    /**
     * Checks that the interface bridge is correctly represented in ifconfig after creation, modification and deletion
     */
    public function test_crud() {
        # Construct a interface bridge object to test with
        $if_bridge = new InterfaceBridge();
        $if_bridge->members->value = ["lan"];
        $if_bridge->create();

        # Ensure the bridge if is correctly shown in ifconfig
        $ifconfig = new Command("ifconfig {$if_bridge->bridgeif->value}");
        $if_create = $if_bridge->members->get_interface_obj()->first()->if->value;
        $this->assert_str_contains($ifconfig->output, "member: $if_create");

        # Update the bridge and ensure the new values are shown in ifconfig and old ones are not
        $if_bridge->members->value = ["wan"];
        $if_bridge->update();
        $ifconfig = new Command("ifconfig {$if_bridge->bridgeif->value}");
        $if_update = $if_bridge->members->get_interface_obj()->first()->if->value;
        $this->assert_str_does_not_contain($ifconfig->output, "membe: $if_create");
        $this->assert_str_contains($ifconfig->output, "member: $if_update");

        # Delete the bridge and ensure it no longer exists
        $if_bridge->delete();
        $this->assert_str_does_not_contain($ifconfig->output, "membe: $if_update");
    }
}