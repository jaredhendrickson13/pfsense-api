<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\PortForward;

class APIModelsPortForwardTestCase extends TestCase
{
    /**
     * Ensures basic port forward rules are correctly set in pfctl after creation, modification and deletion.
     */
    public function test_basic_crud() {
        # Create a port forward to test with
        $port_forward = new PortForward(
            data: [
                "interface" => "lan",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "lan:ip",
                "destination_port" => "8443",
                "target" => "127.1.2.3",
                "local_port" => "4443"
            ],
            async: false
        );
        $port_forward->create(apply: true);

        # Ensure this rule was configured by pfctl
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $if = $port_forward->interface->get_interface_obj()->if->value;
        $proto = $port_forward->protocol->value;
        $src = $port_forward->source->value;
        $dst = $port_forward->interface->get_interface_obj()->get_current_ipv4();
        $dst_port = $port_forward->destination_port->value;
        $target = $port_forward->target->value;
        $local_port = $port_forward->local_port->value;
        $rule_created = "rdr on $if inet proto $proto from $src to $dst port $dst_port -> $target port $local_port";
        $this->assert_str_contains(
            $rules_debug,
            $rule_created
        );
        
        # Update the port forward rule with new values
        $port_forward->from_representation(
            data: [
                "interface" => "lan",
                "protocol" => "tcp/udp",
                "source" => "127.1.2.3",
                "source_port" => "123:125",
                "destination" => "lan",
                "destination_port" => "8443:8445",
                "target" => "127.1.2.3",
                "local_port" => "8443"
            ]
        );
        $port_forward->update(apply: true);
        
        # Ensure this rule was configured by pfctl
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $if = $port_forward->interface->get_interface_obj()->if->value;
        $src = $port_forward->source->value;
        $src_port = $port_forward->source_port->value;
        $dst = $port_forward->interface->get_interface_obj()->get_network_ipv4()."/".$port_forward->interface->get_interface_obj()->get_current_subnetv4();
        $dst_port = $port_forward->destination_port->value;
        $target = $port_forward->target->value;
        $rule_updated = "rdr on $if inet proto { tcp udp } from $src port $src_port to $dst port $dst_port -> $target";
        
        # Ensure the previous pfctl is no longer present and the updated rule is now present
        $this->assert_str_does_not_contain(
            $rules_debug,
            $rule_created
        );
        $this->assert_str_contains(
            $rules_debug,
            $rule_updated
        );
        
        # Delete the port forward and ensure it is no longer present in the pfctl rules
        $port_forward->delete(apply: true);
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain(
            $rules_debug,
            $rule_updated
        );
    }
    
    /**
     * Ensures the configured `natreflection` mode is properly set in the pfctl rules 
     */
}
