<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\WakeOnLANSend;

class APIModelsWakeOnLANSendTestCase extends TestCase
{
    /**
     * Checks that the assigned `interface` must have a valid IPv4 broadcast address.
     */
    public function test_interface_must_have_broadcast_ip(): void {
        # Create an interface without an IP
        $if = new NetworkInterface(
            data: [
                "if" => "em2",
                "descr" => "test",
                "typev4" => "none",
                "typev6" => "none"
            ]
        );
        $if->create();

        # Ensure an error is thrown if we try to send a WOL packet via the interface with no ip
        $this->assert_throws_response(
            response_id: "WAKE_ON_LAN_SEND_INTERFACE_MUST_HAVE_BROADCAST_ADDRESS",
            code: 400,
            callable: function () use ($if) {
                $wol = new WakeOnLANSend();
                $wol->interface->value = $if->id;
                $wol->validate_interface($if->id);
            }
        );

        # Delete the interface
        $if->delete(apply: true);
    }

    /**
     * Checks that the _create() method throws an error if the wol command fails for any reason.
     */
    public function test_error_thrown_if_wol_cmd_fails(): void {
        # Ensure an error is thrown if the wol command fails (i.e. the mac address is somehow invalid, etc.)
        $this->assert_throws_response(
            response_id: "WAKE_ON_LAN_SEND_COMMAND_FAILED",
            code: 500,
            callable: function () {
                $wol = new WakeOnLANSend();
                $wol->interface->value = "lan";
                $wol->mac_addr->value = "invalid_mac_to_trigger_cmd_failure";
                $wol->_create();
            }
        );

        # Ensure no error is thrown when the wol command is successful
        $this->assert_does_not_throw(
            callable: function () {
                $wol = new WakeOnLANSend();
                $wol->interface->value = "lan";
                $wol->mac_addr->value = "00:00:00:00:00:00";
                $wol->_create();
            }
        );
    }
}