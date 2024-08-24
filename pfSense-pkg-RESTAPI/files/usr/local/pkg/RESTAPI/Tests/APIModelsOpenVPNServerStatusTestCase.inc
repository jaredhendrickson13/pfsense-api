<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\OpenVPNServerStatus;

class APIModelsOpenVPNServerStatusTestCase extends TestCase
{
    const MOCK_DATA = [
        [
            "port" => "1194",
            "mode" => "server_user",
            "name" => "Example VPN TCP4:1194",
            "conns" => [
                [
                    "common_name" => "test.example.com",
                    "remote_host" => "127.0.0.2:11712",
                    "virtual_addr" => "127.0.200.2",
                    "virtual_addr6" => "1234::2",
                    "bytes_recv" => "7449",
                    "bytes_sent" => "13963",
                    "connect_time" => "2024-08-23 23:28:08",
                    "connect_time_unix" => "1724477288",
                    "user_name" => "example_user",
                    "client_id" => "29",
                    "peer_id" => "0",
                    "cipher" => "AES-256-GCM"
                ]
            ],
            "vpnid" => "1",
            "mgmt" => "server1",
            "routes" => [
                [
                    "virtual_addr" => "127.0.200.2",
                    "common_name" => "test.example.com",
                    "remote_host" => "1.2.3.4:11712",
                    "last_time" => "2024-08-23 23:28:14"
                ]
            ]
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
        $server_status_q = OpenVPNServerStatus::read_all();
        $this->assert_is_true($server_status_q->exists());

        # Ensure the statuses are correct
        $server_status = $server_status_q->first();
        $this->assert_equals($server_status->name->value, "Example VPN TCP4:1194");
        $this->assert_equals($server_status->port->value, "1194");
        $this->assert_equals($server_status->mode->value, "server_user");
        $this->assert_equals($server_status->vpnid->value, 1);
        $this->assert_equals($server_status->mgmt->value, "server1");
        $this->assert_equals(
            $server_status->conns->value[0],
            [
                "parent_id" => 0,
                "id" => 0,
                "common_name" => "test.example.com",
                "remote_host" => "127.0.0.2:11712",
                "virtual_addr" => "127.0.200.2",
                "virtual_addr6" => "1234::2",
                "bytes_recv" => 7449,
                "bytes_sent" => 13963,
                "connect_time" => "2024-08-23 23:28:08",
                "connect_time_unix" => 1724477288,
                "user_name" => "example_user",
                "client_id" => 29,
                "peer_id" => 0,
                "cipher" => "AES-256-GCM"
            ]
        );
        $this->assert_equals(
            $server_status->routes->value[0],
            [
                "parent_id" => 0,
                "id" => 0,
                "common_name" => "test.example.com",
                "remote_host" => "1.2.3.4:11712",
                "virtual_addr" => "127.0.200.2",
                "last_time" => "2024-08-23 23:28:14"
            ]
        );

        # Reset the mock internal data
        $mock_internal_objects = null;
    }
}