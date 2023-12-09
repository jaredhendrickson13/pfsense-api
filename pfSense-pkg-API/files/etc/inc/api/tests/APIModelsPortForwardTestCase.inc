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
    public function test_natreflection()
    {
        # Create a port forward to test with
        $port_forward = new PortForward(
            data: [
                "interface" => "wan",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "127.3.2.1",
                "destination_port" => "8443",
                "target" => "127.1.2.3",
                "local_port" => "8443",
                "natreflection" => "purenat"
            ],
            async: false
        );
        $port_forward->create(apply: true);

        # Ensure the pure NAT reflection rule is present
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $purenat_rule = "rdr on em1 inet proto tcp from any to 127.3.2.1 port 8443 -> 127.1.2.3";
        $this->assert_str_contains(
            $rules_debug,
            $purenat_rule
        );

        # Update the NAT reflection mode NAT + proxy (enable)
        $port_forward->natreflection->value = "enable";
        $port_forward->update(apply: true);

        # Ensure the pure NAT reflection rule is no longer present and the NAT proxy rule is present
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $natproxy_rule = "rdr on em1 proto tcp from any to 127.3.2.1 port 8443 tag PFREFLECT -> 127.0.0.1 port 19000";
        $this->assert_str_does_not_contain(
            $rules_debug,
            $purenat_rule
        );
        $this->assert_str_contains(
            $rules_debug,
            $natproxy_rule
        );

        # Delete the port forward and ensure the previous reflection rule is no longer present
        $port_forward->delete(apply: true);
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain(
            $rules_debug,
            $natproxy_rule
        );
    }

    /**
     * Checks that port forwards with `nordr` enabled are set as such in pfctl
     */
    public function test_nordr() {
        # Create a port forward to test with
        $port_forward = new PortForward(
            data: [
                "interface" => "wan",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "127.3.2.1",
                "destination_port" => "8443",
                "target" => "127.1.2.3",
                "local_port" => "8443",
                "nordr" => true
            ],
            async: false
        );
        $port_forward->create(apply: true);

        # Ensure this rule was created in pfctl with `no` prefixed to the rule
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $nordr_rule = "no rdr on em0 inet proto tcp from any to 127.3.2.1 port 8443";
        $this->assert_str_contains(
            $rules_debug,
            $nordr_rule
        );

        # Delete this port forward
        $port_forward->delete(apply: true);
    }

    /**
     * Checks that disabled port forwards are not present in pfctl
     */
    public function test_disabled() {
        # Create a port forward to test with
        $port_forward = new PortForward(
            data: [
                "interface" => "wan",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "127.3.2.1",
                "destination_port" => "8443",
                "target" => "127.1.2.3",
                "local_port" => "8443",
                "disabled" => true
            ],
            async: false
        );
        $port_forward->create(apply: true);

        # Ensure this rule's tracker ID is not present in pfctl rules
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain(
            $rules_debug,
            $port_forward->destination->value
        );

        # Delete this port forward
        $port_forward->delete(apply: true);
    }
}
