<?php

namespace API\Tests;

use API\Core\Model;
use API\Core\TestCase;
use API\Models\OutboundNATMapping;

class APIModelsOutboundNATMappingTestCase extends TestCase
{
    /**
     * Checks that outbound NAT mappings are correctly shown in pfctl when created, updated and deleted.
     */
    public function test_crud() {
        # Mock setting the outbound NAT mode
        # TODO: replace with an OutboundNATMode model call when developed 
        Model::set_config("nat/outbound/mode", "hybrid");
        
        # Create a outbound nat rule to test with
        $out_nat = new OutboundNATMapping(
            data: [
                "interface" => "wan",
                "source" => "any",
                "destination" => "127.5.0.1/24",
                "target" => "wan:ip"
            ],
            async: false
        );
        $out_nat->create(apply: true);
        
        # Ensure this outbound NAT rule is now present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $if = $out_nat->interface->get_interface_obj()->descr->value;
        $src = $out_nat->source->value;
        $dst = $out_nat->destination->value;
        $target = $out_nat->interface->get_interface_obj()->get_current_ipv4();
        $nat_rule_created = "nat on \$$if inet from $src to $dst -> $target/32 port 1024:65535";
        $this->assert_str_contains($pfctl_rules, $nat_rule_created);
        
        # Update the outbound NAT mapping with new values
        $out_nat->from_representation(
            data: [
                "interface" => "lan",
                "source" => "lan",
                "destination" => "127.5.0.1/24",
                "target" => "127.3.0.0",
                "target_subnet" => 24,
                "nat_port" => "2000:3000",
                "poolopts" => "round-robin"
            ]
        );
        $out_nat->update(apply: true);
        
        # Ensure the previous NAT rule is no longer present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain($pfctl_rules, $nat_rule_created);

        # Ensure this outbound NAT rule is now present in the pfctl rules
        $if = $out_nat->interface->get_interface_obj()->descr->value;
        $dst = $out_nat->destination->value;
        $target = $out_nat->target->value."/".$out_nat->target_subnet->value;
        $nat_port = $out_nat->nat_port->value;
        $poolopt = $out_nat->poolopts->value;
        $nat_rule_updated = "nat on \$$if inet from \${$if}__NETWORK to $dst -> $target port $nat_port $poolopt";
        $this->assert_str_contains($pfctl_rules, $nat_rule_updated);
        
        # Delete the outbound NAT rule
        $out_nat->delete(apply: true);

        # Ensure the previous NAT rule is no longer present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain($pfctl_rules, $nat_rule_updated);
    }

}
