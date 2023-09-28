<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\RoutingGateway;
use API\Models\StaticRoute;

/**
 * Defines tests for the RoutingGateway Model class.
 */
class APIModelsRoutingGatewayTestCase extends TestCase
{
    /**
     * Checks that RoutingGateways cannot be disabled while its in use.
     */
    public function test_cannot_disable_gateway_while_in_use()
    {
        # TODO: Need tests to ensure gateway cannot be deleted while in use by gw groups and dns servers
        # Create a routing gateway to test with
        $test_gateway = new RoutingGateway(
            representation_data: [
                "name" => "TEST_GW",
                "ipprotocol" => "inet",
                "interface" => "lan",
                "gateway" => "192.168.1.10"
            ]
        );
        $test_gateway->create(apply: true);

        # Create a StaticRoute for this gateway
        $test_route = new StaticRoute(
            representation_data: [
                "network" => "1.2.3.4/32",
                "gateway" => "TEST_GW"
            ]
        );
        $test_route->create(apply: true);

        # Ensure gateway cannot be disabled while the static route is enabled
        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_DISABLED_NOT_ALLOWED_WITH_EXISTING_ROUTE",
            code: 400,
            callable: function () use ($test_gateway) {
                $test_gateway->validate_disabled(disabled: true);
            }
        );

        # Disable the static route
        $test_route->disabled->value= true;
        $test_route->update(apply: true);

        # Ensure the gateway can now be disabled
        $this->assert_does_not_throw(
            callable: function () use ($test_gateway) {
                $test_gateway->validate_disabled(disabled: true);
            }
        );

        # Delete the static route and gateway
        var_dump($test_gateway->id);
        $test_route->delete(apply: true);
        $test_gateway->delete(apply: true);
    }
}