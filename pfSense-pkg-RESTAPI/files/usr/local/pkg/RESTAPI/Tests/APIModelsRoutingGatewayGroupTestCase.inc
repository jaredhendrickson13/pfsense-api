<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\RoutingGatewayGroup;
use RESTAPI\Models\VirtualIP;

class APIModelsRoutingGatewayGroupTestCase extends TestCase {
    /**
     * Ensures that RoutingGatewayGroup objects can be created, updated and deleted.
     */
    public function test_crud(): void {
        # First, create a RoutingGateway object and VirtualIP object we can test with
        $gateway = new RoutingGateway(
            name: 'testgw',
            interface: 'wan',
            ipprotocol: 'inet',
            gateway: '1.2.3.4',
            nonlocalgateway: true,
            async: false,
        );
        $gateway->create();
        $vip = new VirtualIP(
            name: 'testvip',
            interface: 'wan',
            mode: 'ipalias',
            subnet: '1.2.3.4',
            subnet_bits: 24,
            async: false,
        );
        $vip->create(apply: true);

        # Create a gateway group object
        $gateway_group = new RoutingGatewayGroup(
            name: 'testgwgroup',
            trigger: 'down',
            descr: 'Test Gateway Group',
            priorities: [['gateway' => $gateway->name->value, 'tier' => 1, 'virtual_ip' => $vip->uniqid->value]],
            async: false,
        );
        $gateway_group->create(apply: true);

        # Ensure the gateway group object was created
        $pfctl_output = file_get_contents('/tmp/rules.debug');
        $name = $gateway_group->name->value;
        $gw = $gateway->gateway->value;
        $if = $gateway->interface->get_related_model()->if->value;
        $this->assert_str_contains($pfctl_output, "GW$name = \"  route-to { ( $if $gw )  }  \"");

        # Ensure the IP protocol is set correctly
        $this->assert_equals($gateway_group->ipprotocol->value, 'inet');

        # Update the gateway group object with new values
        $gateway_group->trigger->value = 'downloss';
        $gateway_group->descr->value = 'Updated Gateway Group';
        $gateway_group->priorities->value = [
            ['gateway' => $gateway->name->value, 'tier' => 2, 'virtual_ip' => 'address'],
        ];
        $gateway_group->update(apply: true);

        # Delete the gateway group object and ensure it was deleted
        $gateway_group->delete(apply: true);
        $pfctl_output = file_get_contents('/tmp/rules.debug');
        $this->assert_str_does_not_contain($pfctl_output, "GW{$gateway_group->name->value}");

        # Delete the gateway and VIP objects
        $gateway->delete(apply: true);
        $vip->delete(apply: true);
    }

    /**
     * Ensures the 'priorities' field requires at least one priority.
     */
    public function test_priorities_required(): void {
        $this->assert_throws_response(
            response_id: 'LENGTH_VALIDATOR_MINIMUM_CONSTRAINT',
            code: 400,
            callable: function () {
                # Try to create a gateway group object with no priorities
                $gwg = new RoutingGatewayGroup(
                    name: 'testgwgroup',
                    trigger: 'down',
                    descr: 'Test Gateway Group',
                    priorities: [],
                );
                $gwg->validate();
            },
        );
    }
}
