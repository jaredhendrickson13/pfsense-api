<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\OutboundNATMapping;
use RESTAPI\Models\OutboundNATMode;

class APIModelsOutboundNATMappingTestCase extends TestCase
{
    /**
     * Checks that outbound NAT mappings are correctly shown in pfctl when created, updated and deleted.
     */
    public function test_crud() {
        # Set the outbound NAT mode for testing
        $nat_mode = new OutboundNATMode(data: ["mode" => "hybrid"], async: false);
        $nat_mode->update(apply: true);
        
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
        $if = $out_nat->interface->get_related_model()->descr->value;
        $src = $out_nat->source->value;
        $dst = $out_nat->destination->value;
        $target = $out_nat->interface->get_related_model()->get_current_ipv4();
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
        $if = $out_nat->interface->get_related_model()->descr->value;
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

    /**
     * Checks that outbound NAT mappings with `nonat` set are correctly represented in the pfctl rules
     */
    public function test_nonat_in_pfctl() {
        # Set the outbound NAT mode for testing
        $nat_mode = new OutboundNATMode(data: ["mode" => "hybrid"], async: false);
        $nat_mode->update(apply: true);

        # Create a outbound nat rule to test with
        $out_nat = new OutboundNATMapping(
            data: [
                "interface" => "wan",
                "source" => "any",
                "destination" => "127.5.0.1/24",
                "nonat" => true
            ],
            async: false
        );
        $out_nat->create(apply: true);

        # Ensure the rule is correctly set in pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $if = $out_nat->interface->get_related_model()->descr->value;
        $src = $out_nat->source->value;
        $dst = $out_nat->destination->value;
        $this->assert_str_contains(
            $pfctl_rules,
            "no nat on \$$if inet from $src to $dst"
        );

        # Delete the outbound NAT rule
        $out_nat->delete(apply: true);
    }

    /**
     * Checks that outbound NAT mappings with `static_nat_port` set are correctly represented in the pfctl rules
     */
    public function test_static_nat_port_in_pfctl() {
        # Set the outbound NAT mode for testing
        $nat_mode = new OutboundNATMode(data: ["mode" => "hybrid"], async: false);
        $nat_mode->update(apply: true);

        # Create a outbound nat rule to test with
        $out_nat = new OutboundNATMapping(
            data: [
                "interface" => "wan",
                "source" => "any",
                "destination" => "127.5.0.1/24",
                "target" => "127.6.0.1",
                "target_subnet" => 24,
                "static_nat_port" => true
            ],
            async: false
        );
        $out_nat->create(apply: true);

        # Ensure the rule is correctly set in pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $if = $out_nat->interface->get_related_model()->descr->value;
        $src = $out_nat->source->value;
        $dst = $out_nat->destination->value;
        $target = $out_nat->target->value."/".$out_nat->target_subnet->value;
        $this->assert_str_contains(
            $pfctl_rules,
            "nat on \$$if inet from $src to $dst -> $target  static-port"
        );

        # Delete the outbound NAT rule
        $out_nat->delete(apply: true);
    }

    /**
     * Checks that outbound NAT mappings with a `source_hash_key` set is correctly represented in the pfctl rules.
     */
    public function test_source_hash_key() {
        # Set the outbound NAT mode for testing
        $nat_mode = new OutboundNATMode(data: ["mode" => "hybrid"], async: false);
        $nat_mode->update(apply: true);

        # Create a outbound nat rule to test with
        $out_nat = new OutboundNATMapping(
            data: [
                "interface" => "wan",
                "source" => "any",
                "destination" => "127.5.0.1/24",
                "target" => "127.6.0.1",
                "target_subnet" => 24,
                "poolopts" => "source-hash"
            ],
            async: false
        );
        $out_nat->create(apply: true);

        # Ensure the rule is correctly set in pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $if = $out_nat->interface->get_related_model()->descr->value;
        $src = $out_nat->source->value;
        $dst = $out_nat->destination->value;
        $target = $out_nat->target->value."/".$out_nat->target_subnet->value;
        $poolopt = $out_nat->poolopts->value;
        $hash = $out_nat->source_hash_key->value;
        $this->assert_str_contains(
            $pfctl_rules,
            "nat on \$$if inet from $src to $dst -> $target port 1024:65535 $poolopt $hash"
        );

        # Delete the outbound NAT rule
        $out_nat->delete(apply: true);
    }

    /**
     * Checks that `poolopt` cannot be set unless `target` is an alias or IP address
     */
    public function test_poolopts_must_have_supporting_target() {
        # Create an alias and outbound NAT mapping to test with
        $alias = new FirewallAlias(data: ["name" => "test_alias", "type" => "host"]);
        $alias->create();
        $out_nat = new OutboundNATMapping();

        # Ensure an error is thrown if `poolopts` is set when `target` is an interface IP
        $this->assert_throws_response(
            response_id: "OUTBOUND_NAT_MAPPING_POOLOPTS_SET_WITHOUT_MULTI_HOST_TARGET",
            code: 400,
            callable: function () use ($out_nat) {
                $out_nat->from_representation(
                    data: [
                        "interface" => "wan",
                        "source" => "any",
                        "destination" => "127.5.0.1/24",
                        "target" => "wan:ip",
                        "poolopts" => "round-robin"
                    ]
                );
                $out_nat->validate();
            }
        );

        # Ensure an error is not thrown if `poolopts` is set when `target` is an IP address
        $this->assert_does_not_throw(
            callable: function () use ($out_nat) {
                $out_nat->from_representation(
                    data: [
                        "interface" => "wan",
                        "source" => "any",
                        "destination" => "127.5.0.1/24",
                        "target" => "127.5.0.1",
                        "target_subnet" => 24,
                        "poolopts" => "round-robin"
                    ]
                );
                $out_nat->validate();
            }
        );

        # Ensure an error is not thrown if `poolopts` is set when `target` is an alias
        $this->assert_does_not_throw(
            callable: function () use ($out_nat) {
                $out_nat->from_representation(
                    data: [
                        "interface" => "wan",
                        "source" => "any",
                        "destination" => "127.5.0.1/24",
                        "target" => "test_alias",
                        "poolopts" => "round-robin"
                    ]
                );
                $out_nat->validate();
            }
        );

        # Delete the alias we created for testing
        $alias->delete(apply: true);
    }

    /**
     * Ensures an error is thrown if we try to set `target_subnet` to a value greater than 32 when `target` is an IPv4
     * address
     */
    public function test_ipv4_targets_must_have_valid_subnet() {
        $this->assert_throws_response(
            response_id: "OUTBOUND_NAT_MAPPING_TARGET_SUBNET_TOO_LARGE_FOR_IPV4",
            code: 400,
            callable: function () {
                $nat_map = new OutboundNATMapping(
                    data: [
                        "interface" => "wan",
                        "source" => "any",
                        "destination" => "127.5.0.1/24",
                        "target" => "1.2.3.4",
                        "target_subnet" => 33
                    ]
                );
                $nat_map->validate();
            }
        );
    }

    /**
     * Ensures the `target_subnet` field is unset if `target` is not an IP or alias
     */
    public function test_no_target_subnet_if_target_is_not_ip_or_alias() {
        # Ensure non IP or alias `target` values set to `target_subnet` to `null`
        $nat_map = new OutboundNATMapping(
            data: [
                "interface" => "wan",
                "source" => "any",
                "destination" => "127.5.0.1/24",
                "target" => "wan:ip",
                "target_subnet" => 128
            ]
        );
        $nat_map->validate();
        $this->assert_equals($nat_map->target_subnet->value, null);

        # Ensure IP address `target` values retain the `target_subnet`
        $nat_map->target->value = "127.0.0.1";
        $nat_map->target_subnet->value = 32;
        $nat_map->validate();
        $this->assert_equals($nat_map->target_subnet->value, 32);
    }

}
