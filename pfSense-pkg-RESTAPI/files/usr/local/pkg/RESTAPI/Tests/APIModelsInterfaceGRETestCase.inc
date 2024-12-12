<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\InterfaceGRE;

class APIModelsInterfaceGRETestCase extends TestCase
{
    /**
     * Ensure that either a `tunnel_local_addr` or `tunnel_local_addr6` is required.
     */
    public function test_tunnel_local_addr_or_tunnel_local_addr6_required(): void {
        # Ensure an error is thrown if no local address is provided
        $this->assert_throws_response(
            response_id: "INTERFACE_GRE_HAS_NO_LOCAL_ADDRESS",
            code: 400,
            callable: function () {
                $gre = new InterfaceGRE();
                $gre->validate_extra();
            }
        );

        # Ensure no error is thrown in a tunnel_local_addr is assigned
        $this->assert_does_not_throw(
            callable: function () {
                $gre = new InterfaceGRE(tunnel_local_addr: "1.2.3.4");
                $gre->validate_extra();
            }
        );

        # Ensure no error is thrown in a tunnel_local_addr6 is assigned
        $this->assert_does_not_throw(
            callable: function () {
                $gre = new InterfaceGRE(tunnel_local_addr6: "1234::1");
                $gre->validate_extra();
            }
        );
    }

    /**
     * Ensure an existing GRE cannot be deleted if it is in use.
     */
    public function test_delete_in_use(): void
    {
        # Ensure an error is thrown if the GRE is in use
        $this->assert_throws_response(
            response_id: "INTERFACE_GRE_CANNOT_DELETE_WHILE_IN_USE",
            code: 409,
            callable: function () {
                $gre = new InterfaceGRE(
                    if: "lan",
                    remote_addr: "1.2.3.4",
                    tunnel_local_addr: "4.3.2.1",
                    tunnel_remote_addr: "1.1.2.2",
                    tunnel_remote_net: 32
                );
                $gre->create();

                # Mock a GRE interface in use
                InterfaceGRE::set_config("interfaces/opt99", ["if" => $gre->greif->value]);
                $gre->delete();
            }
        );

        # Remove all GRE interfaces used in the test
        InterfaceGRE::del_config("interfaces/opt99");
        InterfaceGRE::delete_all();
    }

    /**
     * Ensure we can create, read, update and delete a GRE interface.
     */
    public function test_crud(): void {
        # Create a new GRE tunnel and ensure it's interface is seen in ifconfig
        $gre = new InterfaceGRE(
            if: "lan",
            remote_addr: "1.2.3.4",
            tunnel_local_addr: "4.3.2.1",
            tunnel_remote_addr: "1.1.2.2",
            tunnel_remote_net: 32
        );
        $gre->create();
        $ifconfig_output = new Command("ifconfig");
        $this->assert_str_contains($ifconfig_output->output, $gre->greif->value);
        $this->assert_str_contains($ifconfig_output->output, "tunnel inet 192.168.1.1 --> 1.2.3.4");
        $this->assert_str_contains($ifconfig_output->output, "inet 4.3.2.1 --> 1.1.2.2 netmask 0xffffffff");

        # Update the GRE tunnel and ensure the changes are reflected in ifconfig
        $gre->remote_addr->value = "2.3.4.5";
        $gre->update();
        $ifconfig_output = new Command("ifconfig");
        $this->assert_str_does_not_contain($ifconfig_output->output, "tunnel inet 192.168.1.1 --> 1.2.3.4");
        $this->assert_str_contains($ifconfig_output->output, "tunnel inet 192.168.1.1 --> 2.3.4.5");

        # Delete the GRE tunnel and ensure it's interface is no longer seen in ifconfig
        $gre->delete();
        $ifconfig_output = new Command("ifconfig");
        $this->assert_str_does_not_contain($ifconfig_output->output, $gre->greif->value);


    }
}