<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DefaultGateway;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\RoutingGatewayGroup;

class APIModelsDefaultGatewayTestCase extends TestCase {
    /**
     * @var RoutingGateway $gateway The RoutingGateway object we will use for testing.
     */
    private RoutingGateway $gateway;

    /**
     * @var RoutingGatewayGroup $gateway_group The RoutingGatewayGroup object we will use for testing.
     */
    private RoutingGatewayGroup $gateway_group;

    /**
     * Setup the test case by creating a new RoutingGateway and GatewayGroup we can use for testing.
     */
    public function setup(): void {
        # Create a gateway object we can use for testing
        $this->gateway = new RoutingGateway(
            name: 'testgw',
            interface: 'wan',
            gateway: '1.2.3.4',
            nonlocalgateway: true,
            ipprotocol: 'inet',
        );
        $this->gateway->create();

        # Create a gateway group object we can use for testing
        $this->gateway_group = new RoutingGatewayGroup(
            name: 'testgwgroup',
            trigger: 'down',
            descr: 'Test Gateway Group',
            priorities: [['gateway' => $this->gateway->name->value, 'tier' => 1, 'virtual_ip' => 'address']],
        );
        $this->gateway_group->create();
    }

    /**
     * Tear down the test case by deleting the RoutingGateway and GatewayGroup objects we created.
     */
    public function teardown(): void {
        $this->gateway_group->delete();
        $this->gateway->delete();
    }

    /**
     * Ensure we can update the DefaultGateway for both IPv4 and IPv6
     */
    public function test_update(): void {
        $this->assert_does_not_throw(
            callable: function () {
                $default_gw = new DefaultGateway(defaultgw4: '', defaultgw6: '-');
                $default_gw->update(apply: true);
            },
        );
    }

    /**
     * Ensure we can assign an existing RoutingGateway or RoutingGatewayGroup as the DefaultGateway
     */
    public function test_assign_gateway_or_gateway_group(): void {
        $this->assert_does_not_throw(
            callable: function () {
                $default_gw = new DefaultGateway(defaultgw4: $this->gateway->name->value);
                $default_gw->validate();

                $default_gw = new DefaultGateway(defaultgw4: $this->gateway_group->name->value);
                $default_gw->validate();
            },
        );
    }
}
