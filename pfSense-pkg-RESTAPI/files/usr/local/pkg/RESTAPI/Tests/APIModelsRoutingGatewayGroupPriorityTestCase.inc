<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\RoutingGatewayGroup;
use RESTAPI\Models\RoutingGatewayGroupPriority;
use RESTAPI\Models\VirtualIP;

class APIModelsRoutingGatewayGroupPriorityTestCase extends TestCase
{
    /**
     * Ensures that RoutingGatewayGroup objects can be created, updated and deleted.
     */
    public function test_crud(): void {
        # First, create a RoutingGateway object and VirtualIP object we can test with
        $gateway = new RoutingGateway(
            name: "testgw",
            interface: "wan",
            ipprotocol: "inet",
            gateway: "1.2.3.4",
            nonlocalgateway: true,
            async: false
        );
        $gateway->create();

        # Create a gateway group object
        $gateway_group = new RoutingGatewayGroup(
            name: "testgwgroup",
            trigger: "down",
            descr: "Test Gateway Group",
            priorities: [
                ['gateway' => $gateway->name->value, 'tier' => 1]
            ],
            async: false
        );
        $gateway_group->create();

        # Create a new gateway and ensure we can add it to the gateway group
        $gateway2 = new RoutingGateway(
            name: "testgw2",
            interface: "wan",
            ipprotocol: "inet",
            gateway: "4.3.2.1",
            nonlocalgateway: true
        );
        $gateway2->create();

        # Add the new priority to the gateway group
        $gateway_prio = new RoutingGatewayGroupPriority(
            parent_id: $gateway_group->id,
            gateway: $gateway2->name->value,
            tier: 2
        );
        $gateway_prio->create();

        # Ensure the gateway group object was created
        $this->assert_equals(RoutingGatewayGroupPriority::read_all(parent_id: $gateway_group->id)->count(), 2);

        # Update the gateway group priority object with new values
        $gateway_prio->tier->value = 3;
        $gateway_prio->update();

        # Delete the gateway group priority object and ensure it was deleted
        $gateway_prio->delete();
        $this->assert_equals(RoutingGatewayGroupPriority::read_all(parent_id: $gateway_group->id)->count(), 1);

        # Cleanup
        $gateway_group->delete();
        $gateway2->delete();
        $gateway->delete();
    }

    /**
     * Ensures an error is thrown if we try to add an IPv6 gateway to an IPv4 gateway group.
     */
    public function test_ipprotocol_mismatch(): void {
        # Create IPv4 and IPv6 gateways
        $gatewayv4 = new RoutingGateway(
            name: "testgwv4",
            interface: "wan",
            ipprotocol: "inet",
            gateway: "1.2.3.4",
            nonlocalgateway: true,
            async: false
        );
        $gatewayv6 = new RoutingGateway(
            name: "testgwv6",
            interface: "wan",
            ipprotocol: "inet6",
            gateway: "1234::1",
            nonlocalgateway: true,
            async: false
        );
        $gatewayv4->create();
        $gatewayv6->create();

        # Create a gateway group object
        $gateway_group = new RoutingGatewayGroup(
            name: "testgwgroup",
            trigger: "down",
            descr: "Test Gateway Group",
            priorities: [
                ['gateway' => $gatewayv4->name->value, 'tier' => 1]
            ],
            async: false
        );
        $gateway_group->create();

        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_GROUP_PRIORITY_IPPROTOCOL_MISMATCH",
            code: 400,
            callable: function () use ($gateway_group, $gatewayv6){
                $gateway_prio = new RoutingGatewayGroupPriority(
                    parent_id: $gateway_group->id,
                    gateway: $gatewayv6->name->value,
                    tier: 2
                );
                $gateway_prio->validate();
            }
        );

        # Cleanup
        $gateway_group->delete();
        $gatewayv6->delete();
        $gatewayv4->delete();
    }

    /**
     * Ensures an error is thrown if the internal gateway group priority object is malformed.
     */
    public function test_invalid_internal_gateway_group_priority(): void {
        # Ensure invalid internal values thrown an error
        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_GROUP_PRIORITY_INTERNAL_OBJECT_INVALID",
            code: 500,
            callable: function () {
                $gateway_prio = new RoutingGatewayGroupPriority();
                $gateway_prio->from_internal_object("invalid");
            }
        );
        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_GROUP_PRIORITY_INTERNAL_OBJECT_INVALID",
            code: 500,
            callable: function () {
                $gateway_prio = new RoutingGatewayGroupPriority();
                $gateway_prio->from_internal_object("invalid|invalid");
            }
        );
        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_GROUP_PRIORITY_INTERNAL_OBJECT_INVALID",
            code: 500,
            callable: function () {
                $gateway_prio = new RoutingGatewayGroupPriority();
                $gateway_prio->from_internal_object("invalid|invalid|invalid|invalid");
            }
        );
        $this->assert_throws_response(
            response_id: "ROUTING_GATEWAY_GROUP_PRIORITY_INTERNAL_OBJECT_INVALID",
            code: 500,
            callable: function () {
                $gateway_prio = new RoutingGatewayGroupPriority();
                $gateway_prio->from_internal_object(["invalid"]);
            }
        );

        # Ensure valid internal values correctly populate the object
        $gateway_prio = new RoutingGatewayGroupPriority();
        $gateway_prio->from_internal_object("testgw|2|address");
        $this->assert_equals($gateway_prio->gateway->value, "testgw");
        $this->assert_equals($gateway_prio->tier->value, 2);
        $this->assert_equals($gateway_prio->virtual_ip->value, "address");
    }

    /**
     * Ensures the to_internal() method correctly converts the object to its internal form.
     */
    public function test_to_internal(): void {
        $gateway_prio = new RoutingGatewayGroupPriority(
            gateway: "testgw",
            tier: 1,
            virtual_ip: "address"
        );
        $this->assert_equals($gateway_prio->to_internal(), "testgw|1|address");
    }

}