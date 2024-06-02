<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\NetworkInterface;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\StaticRoute;

/**
 * Defines Tests for the RoutingGateway Model class.
 */
class APIModelsRoutingGatewayTestCase extends TestCase {
    /**
     * Checks that RoutingGateways cannot be disabled while its in use.
     */
    public function test_cannot_disable_gateway_while_in_use() {
        # TODO: Need Tests to ensure gateway cannot be disabled while in use by gw groups and dns servers
        # Create a routing gateway to test with
        $test_gateway = new RoutingGateway(
            data: [
                'name' => 'TEST_GW',
                'ipprotocol' => 'inet',
                'interface' => 'lan',
                'gateway' => '192.168.1.10',
            ],
            async: false,
        );
        $test_gateway->create(apply: true);

        # Create a StaticRoute for this gateway
        $test_route = new StaticRoute(
            data: [
                'network' => '1.2.3.4/32',
                'gateway' => 'TEST_GW',
            ],
            async: false,
        );
        $test_route->create(apply: true);

        # Ensure gateway cannot be disabled while the static route is enabled
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_DISABLED_NOT_ALLOWED_WITH_EXISTING_ROUTE',
            code: 400,
            callable: function () use ($test_gateway) {
                $test_gateway->validate_disabled(disabled: true);
            },
        );

        # Disable the static route
        $test_route->disabled->value = true;
        $test_route->update(apply: true);

        # Ensure the gateway can now be disabled
        $this->assert_does_not_throw(
            callable: function () use ($test_gateway) {
                $test_gateway->validate_disabled(disabled: true);
            },
        );

        # Delete the static route and gateway
        $test_route->delete(apply: true);
        $test_gateway->delete(apply: true);
    }

    /**
     * Ensure a gateway's interface has an IP of the assigned `ipprotocol`
     */
    public function test_gateway_interface_must_match_gateway_ip_protocol() {
        # Create a parent interface to test with
        $test_if = new NetworkInterface(
            data: [
                'enable' => true,
                'if' => $this->env['PFREST_OPT1_IF'],
                'descr' => 'TEST_IFACE',
                'typev4' => 'none',
                'typev6' => 'none',
            ],
            async: false,
        );
        $test_if->create();

        # Ensure an IPv4 gateway cannot be created when the interface has no IPv4 address
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_INTERFACE_HAS_NO_IPV4',
            code: 400,
            callable: function () use ($test_if) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => $test_if->id,
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
        # Ensure an IPv6 gateway cannot be created when the interface has no IPv6 address
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_INTERFACE_HAS_NO_IPV6',
            code: 400,
            callable: function () use ($test_if) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => $test_if->id,
                        'ipprotocol' => 'inet6',
                        'gateway' => '1234::2',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        # Give the interface an IPv4 and IPv6 address
        $test_if->from_representation(
            data: [
                'enabled' => true,
                'descr' => 'TEST_IFACE',
                'typev4' => 'static',
                'ipaddr' => '1.2.3.1',
                'subnet' => 24,
                'typev6' => 'staticv6',
                'ipaddrv6' => '1234::1',
                'subnetv6' => 64,
            ],
        );
        $test_if->update();

        # Ensure an IPv4 gateway can be created now
        $this->assert_does_not_throw(
            callable: function () use ($test_if) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => $test_if->id,
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
        # Ensure an IPv6 gateway can be created now
        $this->assert_does_not_throw(
            callable: function () use ($test_if) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => $test_if->id,
                        'ipprotocol' => 'inet6',
                        'gateway' => '1234::2',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        $test_if->delete(apply: true);
    }

    /**
     * Checks to ensure the `gateway` field is set to a value that matches the `ipprotocol`.
     */
    public function test_gateway_matches_ipprotocol_type() {
        # Ensure an IPv4 IP cannot be used for IPv6 gateway
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_NO_IPV4_WITH_INET6',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet6',
                        'gateway' => '1.2.3.4',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        # Ensure an IPv6 IP cannot be used for IPv4 gateway
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_NO_IPV6_WITH_INET',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1234::2',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        # Ensure an IPv6 IP can be used for IPv6 gateway
        $this->assert_does_not_throw(
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet6',
                        'gateway' => '1234::2',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        # Ensure an IPv4 IP can be used for IPv4 gateway
        $this->assert_does_not_throw(
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Ensures the `latencyhigh` field cannot be set to a value lower than `latencylow`
     */
    public function test_latencyhigh_must_be_greater_than_latencylow() {
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_WITH_LATENCY_LOW_GREATER_THAN_HIGH',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'latencylow' => 200,
                        'latencyhigh' => 199,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Ensures the `losshigh` field cannot be set to a value lower than `losslow`
     */
    public function test_losshigh_must_be_greater_than_losslow() {
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_WITH_LOSS_LOW_GREATER_THAN_HIGH',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'losslow' => 10,
                        'losshigh' => 9,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Ensures the `loss_interval` field cannot be set to a value lower than `latencyhigh`
     */
    public function test_loss_interval_must_be_greater_than_latencyhigh() {
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_WITH_LOSS_INTERVAL_GREATER_THAN_LATENCY_HIGH',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'loss_interval' => 499,
                        'latencyhigh' => 500,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Checks that the `time_period` must be greater than twice the `interval` plus the `loss_interval`
     */
    public function test_time_period_minimum_constraint() {
        # Calculate the minimum value
        $interval = 500;
        $loss_interval = 2000;
        $minimum = 2 * $interval + $loss_interval;

        # Ensure a value that is exactly twice the interval plus the loss interval is not allowed
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_TIME_PERIOD_UNDER_MINIMUM',
            code: 400,
            callable: function () use ($minimum, $interval, $loss_interval) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'time_period' => $minimum,
                        'interval' => $interval,
                        'loss_interval' => $loss_interval,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );

        # Ensure a value that is greater than twice the interval plus the loss interval is allowed
        $this->assert_does_not_throw(
            callable: function () use ($minimum, $interval, $loss_interval) {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'time_period' => $minimum + 1,
                        'interval' => $interval,
                        'loss_interval' => $loss_interval,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Ensures the `alert_interval` must be greater than or equal to `interval`
     */
    public function test_loss_interval_must_be_greater_than_interval() {
        $this->assert_throws_response(
            response_id: 'ROUTING_GATEWAY_ALERT_INTERVAL_UNDER_MINIMUM',
            code: 400,
            callable: function () {
                $test_gw = new RoutingGateway(
                    data: [
                        'name' => 'TEST_GW',
                        'interface' => 'wan',
                        'ipprotocol' => 'inet',
                        'gateway' => '1.2.3.4',
                        'interval' => 500,
                        'alert_interval' => 499,
                    ],
                    async: false,
                );
                $test_gw->validate();
            },
        );
    }

    /**
     * Checks that the gateway is configured on the backend when the RoutingGateway is created, updated and deleted.
     */
    public function test_gateway_route_found_in_netstat() {
        # Create a new RoutingGateway
        $test_gw = new RoutingGateway(
            data: [
                'name' => 'TEST_GW',
                'interface' => 'lan',
                'ipprotocol' => 'inet',
                'gateway' => '1.2.3.4',
            ],
            async: false,
        );
        $test_gw->create(apply: true);

        # Ensure it is now found in the routing table
        $netstat_output = $this->run_command('netstat -rn')['output'];
        $this->assert_str_contains($netstat_output, '1.2.3.4');

        # Update the RoutingGateway's IP and ensure it is now present in the routing table and 1.2.3.4 is not
        $test_gw->gateway->value = '4.3.2.1';
        $test_gw->update(apply: true);
        $netstat_output = $this->run_command('netstat -rn')['output'];
        $this->assert_str_does_not_contain($netstat_output, '1.2.3.4');
        $this->assert_str_contains($netstat_output, '4.3.2.1');

        # Delete the RoutingGateway and ensure 4.3.2.1 is no longer in the routing table
        $test_gw->delete(apply: true);
        $netstat_output = $this->run_command('netstat -rn')['output'];
        $this->assert_str_does_not_contain($netstat_output, '4.3.2.1');
    }
}
