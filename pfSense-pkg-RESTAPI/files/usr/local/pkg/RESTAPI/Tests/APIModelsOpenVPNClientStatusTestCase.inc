<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\OpenVPNClientStatus;
use RESTAPI\Models\OpenVPNServerStatus;

class APIModelsOpenVPNClientStatusTestCase extends TestCase
{
    const MOCK_DATA = [
        [
            "name" => "Example VPN Client UPD4",
            "port" => "1194",
            "vpnid" => "2",
            "mgmt" => "client1",
            "status" => "Waiting for response from peer",
            "state" => "WAIT",
            "state_detail" => "Waiting",
            "connect_time" => "2024-08-23 23:28:08",
            "virtual_addr" => "127.0.200.2",
            "virtual_addr6" => "1234::2",
            "remote_host" => "1.2.3.4",
            "remote_port" => "11712",
            "local_host" => "4.3.2.1",
            "local_port" => "11712"
        ]
    ];

    /**
     * Ensures we can read OpenVPNServerStatus objects.
     */
    public function testReadOpenVPNServerStatus(): void {
        # Setup the mock internal data so we don't have to actually connect VPNs to read values
        global $mock_internal_objects;
        $mock_internal_objects = self::MOCK_DATA;

        # Read the OpenVPNServerStatus objects
        $client_status_q = OpenVPNClientStatus::read_all();
        $this->assert_is_true($client_status_q->exists());

        # Ensure the statuses are correct
        $client_status = $client_status_q->first();
        $this->assert_equals($client_status->name->value, "Example VPN Client UPD4");
        $this->assert_equals($client_status->port->value, "1194");
        $this->assert_equals($client_status->vpnid->value, 2);
        $this->assert_equals($client_status->mgmt->value, "client1");
        $this->assert_equals($client_status->status->value, "Waiting for response from peer");
        $this->assert_equals($client_status->state->value, "WAIT");
        $this->assert_equals($client_status->state_detail->value, "Waiting");
        $this->assert_equals($client_status->connect_time->value, "2024-08-23 23:28:08");
        $this->assert_equals($client_status->virtual_addr->value, "127.0.200.2");
        $this->assert_equals($client_status->virtual_addr6->value, "1234::2");
        $this->assert_equals($client_status->remote_host->value, "1.2.3.4");
        $this->assert_equals($client_status->remote_port->value, "11712");
        $this->assert_equals($client_status->local_host->value, "4.3.2.1");
        $this->assert_equals($client_status->local_port->value, "11712");

        # Reset the mock internal data
        $mock_internal_objects = null;
    }
}