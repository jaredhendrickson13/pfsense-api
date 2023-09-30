<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\FirewallAlias;
use API\Models\NetworkInterface;
use API\Models\RoutingGateway;
use API\Models\StaticRoute;

class APIModelsStaticRouteTestCase extends TestCase
{
    /**
     * Checks that the static route cannot conflict with an existing interface's subnet.
     */
    public function test_static_route_cannot_conflict_with_interface_subnet() {
        # Create an interface to use for testing
        $test_if = new NetworkInterface(
            representation_data: [
                "if" => "em2",
                "enable" => true,
                "descr" => "TESTIF",
                "typev4" => "static",
                "ipaddr" => "1.2.3.4",
                "subnet" => 24,
                "typev6" => "staticv6",
                "ipaddrv6" => "1234::2",
                "subnetv6" => 64
            ]
        );
        $test_if->create();

        # Create gateways to use for testing
        $test_gw4 = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW4",
                "interface" => $test_if->id,
                "ipprotocol" => "inet",
                "gateway" => "1.2.3.1"
            ]
        );
        $test_gw6 = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW6",
                "interface" => $test_if->id,
                "ipprotocol" => "inet6",
                "gateway" => "1234::1"
            ]
        );
        $test_gw4->create();
        $test_gw6->create();

        # Ensure an IPv4 static route cannot be created for the interface's 1.2.3.0/24 subnet.
        $this->assert_throws_response(
            response_id: "STATIC_ROUTE_CONFLICTS_WITH_IPV4_INTERFACE",
            code: 400,
            callable: function () use ($test_if, $test_gw4) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw4->name->value,
                        "network" => $test_if->ipaddr->value."/".$test_if->subnet->value,
                    ]
                );
                $test_route->validate();
            }
        );
        # Ensure an IPv6 static route cannot be created for the interface's 1234::/64 subnet.
        $this->assert_throws_response(
            response_id: "STATIC_ROUTE_CONFLICTS_WITH_IPV6_INTERFACE",
            code: 400,
            callable: function () use ($test_if, $test_gw6) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw6->name->value,
                        "network" => $test_if->ipaddrv6->value."/".$test_if->subnetv6->value,
                    ]
                );
                $test_route->validate();
            }
        );

        # Ensure routes can be created for subnets that do not conflict with any interface's subnet.
        $this->assert_does_not_throw(
            callable: function () use ($test_gw4) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw4->name->value,
                        "network" => "4.3.2.1/24",
                    ]
                );
                $test_route->validate();
            }
        );
        $this->assert_does_not_throw(
            callable: function () use ($test_gw6) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw6->name->value,
                        "network" => "4321::/64",
                    ]
                );
                $test_route->validate();
            }
        );

        # Remove objects created by this test
        $test_gw6->delete(apply: true);
        $test_gw4->delete(apply: true);
        $test_if->delete(apply: true);
    }

    /**
     * Checks that the static route's network matches the IP type of the gateway.
     */
    public function test_static_route_network_must_match_gateway_ip_type() {
        # Create an interface to use for testing
        $test_if = new NetworkInterface(
            representation_data: [
                "if" => "em2",
                "enable" => true,
                "descr" => "TESTIF",
                "typev4" => "static",
                "ipaddr" => "1.2.3.4",
                "subnet" => 24,
                "typev6" => "staticv6",
                "ipaddrv6" => "1234::2",
                "subnetv6" => 64
            ]
        );
        $test_if->create();

        # Create gateways to use for testing
        $test_gw4 = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW4",
                "interface" => $test_if->id,
                "ipprotocol" => "inet",
                "gateway" => "1.2.3.1"
            ]
        );
        $test_gw6 = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW6",
                "interface" => $test_if->id,
                "ipprotocol" => "inet6",
                "gateway" => "1234::1"
            ]
        );
        $test_gw4->create();
        $test_gw6->create();

        # Ensure an IPv4 static route cannot be created for the interface's 1.2.3.0/24 subnet.
        $this->assert_throws_response(
            response_id: "STATIC_ROUTE_NON_IPV4_NETWORK_WITH_IPV4_GATEWAY",
            code: 400,
            callable: function () use ($test_if, $test_gw4) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw4->name->value,
                        "network" => "1111::1/64",
                    ]
                );
                $test_route->validate();
            }
        );
        # Ensure an IPv6 static route cannot be created for the interface's 1234::/64 subnet.
        $this->assert_throws_response(
            response_id: "STATIC_ROUTE_NON_IPV6_NETWORK_WITH_IPV6_GATEWAY",
            code: 400,
            callable: function () use ($test_if, $test_gw6) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw6->name->value,
                        "network" => "1.1.1.1/24",
                    ]
                );
                $test_route->validate();
            }
        );

        # Remove objects created by this test
        $test_gw6->delete(apply: true);
        $test_gw4->delete(apply: true);
        $test_if->delete(apply: true);
    }

    /**
     * Checks that static routes must be disabled if the parent gateway is disabled.
     */
    public function test_static_route_must_be_disabled_when_gateway_is_disabled() {
        # Create gateways to use for testing
        $test_gw4 = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW4",
                "interface" => "lan",
                "ipprotocol" => "inet",
                "gateway" => "1.2.3.1",
                "disabled" => true
            ]
        );
        $test_gw4->create();

        # Ensure an IPv4 static route cannot be created for the interface's 1.2.3.0/24 subnet.
        $this->assert_throws_response(
            response_id: "STATIC_ROUTE_GATEWAY_DISABLED",
            code: 400,
            callable: function () use ($test_gw4) {
                $test_route = new StaticRoute(
                    representation_data: [
                        "gateway" => $test_gw4->name->value,
                        "network" => "4.3.2.1/24",
                        "disabled" => false
                    ]
                );
                $test_route->validate();
            }
        );

        $test_gw4->delete(apply: true);
    }

    /**
     * Checks that the `get_networks()` method correctly gathers all applicable networks for the given `network` value.
     */
    public function test_get_networks() {
        # Create an alias to use for testing
        $test_alias = new FirewallAlias(
            representation_data: [
                "name" => "TESTALIAS",
                "type" => "network",
                "address" => ["192.168.10.1/24", "192.168.20.1/25", "192.168.30.0/29"]
            ]
        );
        $test_alias->create();

        # Create a gateway to use for testing
        $test_gw = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW",
                "interface" => "lan",
                "ipprotocol" => "inet",
                "gateway" => "192.168.1.10"
            ]
        );
        $test_gw->create();

        # Create a static route to test with
        $test_route = new StaticRoute(
            representation_data: [
                "gateway" => $test_gw->name->value,
                "network" => $test_alias->name->value
            ]
        );

        # Validate the route and check that the output of get_networks() is expected
        $test_route->validate();
        $this->assert_equals($test_route->get_networks(), $test_alias->address->value);

        # Update the alias to be a host alias and check that the output of get_networks() is expected
        $test_alias->type->value = "host";
        $test_alias->address->value = ["192.168.20.2", "192.168.30.3", "192.168.40.4"];
        $test_alias->update();
        $this->assert_equals($test_route->get_networks(), ["192.168.20.2/32", "192.168.30.3/32", "192.168.40.4/32"]);

        # Ensure when `network` is set to a subnet instead of an alias, that get_networks() just returns the subnet
        $test_route->network->value = "192.168.50.0/24";
        $test_route->validate();
        $this->assert_equals($test_route->get_networks(), ["192.168.50.0/24"]);

        # Delete objects created for this test
        $test_alias->delete(apply: true);
        $test_gw->delete(apply: true);
    }

    /**
     * Checks that static routes are found in the routing table after creation, updates and not found after deletion.
     */
    public function test_static_route_configured_in_routing_table() {
        # Create a gateway to use for testing
        $test_gw = new RoutingGateway(
            representation_data: [
                "name" => "TESTGW",
                "interface" => "lan",
                "ipprotocol" => "inet",
                "gateway" => "192.168.1.10"
            ]
        );
        $test_gw->create();

        # Create a static route using the gateway above
        $test_route = new StaticRoute(representation_data:
            ["network" => "1.2.3.0/24", "gateway" => $test_gw->name->value]
        );
        $test_route->create(apply: true);

        # Ensure the route is found in the routing table after applying
        $this->assert_is_true(
            str_contains(
                $this->run_command("netstat -rn", trim_whitespace: true)["output"],
                "{$test_route->network->value} {$test_gw->gateway->value}"
            )
        );

        # Update the static route to change the static route network
        $test_route->network->value = "4.3.2.0/31";
        $test_route->update(apply: true);

        # Ensure the new route is found in the routing table and the old one is not after applying
        $this->assert_is_false(
            str_contains(
                $this->run_command("netstat -rn", trim_whitespace: true)["output"],
                "1.2.3.0/24 {$test_gw->gateway->value}"
            )
        );
        $this->assert_is_true(
            str_contains(
                $this->run_command("netstat -rn", trim_whitespace: true)["output"],
                "{$test_route->network->value} {$test_gw->gateway->value}"
            )
        );

        # Remove the static route and ensure it is no longer present in the routing table
        $test_route->delete(apply: true);

        $this->assert_is_false(
            str_contains(
                $this->run_command("netstat -rn", trim_whitespace: true)["output"],
                "{$test_route->network->value} {$test_gw->gateway->value}"
            )
        );

        # Delete the gateway used for testing
        $test_gw->delete(apply: true);
    }
}
