<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallRule;
use RESTAPI\Models\RoutingGateway;

class APIModelsFirewallRuleTestCase extends TestCase
{
    /**
     * Checks that multiple interfaces cannot be assigned to a FirewallRule unless `floating` is enabled.
     */
    public function test_no_multiple_interface_without_floating() {
        # Ensure multiple interface values are not allowed for non floating FirewallRules
        $this->assert_throws_response(
            response_id: "FIREWALL_RULE_MULTIPLE_INTERFACE_WITHOUT_FLOATING",
            code: 400,
            callable: function () {
                $rule = new FirewallRule();
                $rule->interface->value = ["wan", "lan"];
                $rule->validate_interface("wan");
            }
        );

        # Ensure multiple interface values are allowed for floating FirewallRules
        $this->assert_does_not_throw(
            callable: function () {
                $rule = new FirewallRule();
                $rule->interface->value = ["wan", "lan"];
                $rule->floating->value = true;
                $rule->validate_interface("wan");
            }
        );
    }
    
    /**
     * Checks that the `statetype` can only be `synproxy state` if the `protocol` is `tcp`
     */
    public function test_no_synproxy_statetype_with_non_tcp_rule() {
        $this->assert_throws_response(
            response_id: "FIREWALL_RULE_SYNPROXY_STATE_TYPE_WITH_NON_TCP_PROTOCOL",
            code: 400,
            callable: function () {
                $rule = new FirewallRule();
                $rule->protocol->value = "icmp";
                $rule->validate_statetype("synproxy state");
            }
        );
    }

    /**
     * Checks that the `statetype` can only be `synproxy state` if a `gateway` is not set.
     */
    public function test_no_synproxy_statetype_with_gateway_assigned() {
        $this->assert_throws_response(
            response_id: "FIREWALL_RULE_SYNPROXY_STATE_TYPE_WITH_GATEWAY",
            code: 400,
            callable: function () {
                $rule = new FirewallRule();
                $rule->protocol->value = "tcp";
                $rule->gateway->value = "TESTGW";
                $rule->validate_statetype("synproxy state");
            }
        );
    }
    
    /**
     * Checks that any values specific in `tcp_flags_set` must also be present in `tcp_flags_out_of`
     */
    public function test_tcp_flag_set_must_be_in_tcp_flags_out_of() {
        $this->assert_throws_response(
            response_id: "FIREWALL_RULE_TCP_FLAGS_SET_NOT_IN_TCP_FLAGS_OUT_OF",
            code: 400,
            callable: function () {
                $rule = new FirewallRule();
                $rule->tcp_flags_out_of->value = ["syn", "ack"];
                $rule->validate_tcp_flags_set("rst");
            }
        );
    }
    
    /**
     * Checks that the `update_by` value is automatically updated with the current user regardless of what value
     * is currently is assigned.
     */
    public function test_updated_by_is_automatically_overwritten() {
        # Define a FirewallRule object and set it's client username and IP
        $rule = new FirewallRule();
        $rule->client->username = "testuser1";
        $rule->client->ip_address = "127.0.0.1";
        
        # Ensure the `validate_updated_by()` method automatically generates a new value using the client user and IP
        $this->assert_equals(
            $rule->validate_updated_by("doesn't matter what this value is!"),
            "{$rule->client->username}@{$rule->client->ip_address} (API)"
        );
        
        # For good measure, update the client username and IP again and ensure it is automatically updated
        $rule->client->username = "testuser2";
        $rule->client->ip_address = "127.0.0.2";
        $this->assert_equals(
            $rule->validate_updated_by("doesn't matter what this value is!"),
            "{$rule->client->username}@{$rule->client->ip_address} (API)"
        );
    }

    /**
     * Checks that basic firewall rules are actually configured by pfctl after creation, modification and deletion.
     */
    public function test_basic_crud() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "protocol" => "tcp",
                "source" => "127.0.0.1",
                "source_port" => "80",
                "destination" => "127.0.0.2",
                "destination_port" => "443",
                "descr" => "test description"
            ],
            async: false
        );
        $rule->create(apply: true);

        # Check that this rule is present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $type = $rule->type->value;
        $if = "\${$rule->interface->get_related_model()->first()->descr->value}";
        $iptype = $rule->ipprotocol->value;
        $proto = $rule->protocol->value;
        $src = $rule->source->value;
        $srcport = $rule->source_port->value;
        $dst = $rule->destination->value;
        $dstport = $rule->destination_port->value;
        $descr = $rule->descr->value;
        $rid = $rule->tracker->value;
        $pfctl_rule_created = "$type  in  quick  on $if $iptype proto $proto  from $src port $srcport to $dst port ".
            "$dstport ridentifier $rid flags S/SA keep state label \"USER_RULE: $descr\" label \"id:$rid\"";
        $this->assert_str_contains($pfctl_rules, $pfctl_rule_created);

        # Update the firewall rule object with new values
        $rule->from_representation(
            data: [
                "type" => "block",
                "interface" => ["lan"],
                "ipprotocol" => "inet6",
                "protocol" => "udp",
                "source" => "fe80::1",
                "source_port" => "8080",
                "destination" => "fe80::2",
                "destination_port" => "8443",
                "descr" => "updated test description"
            ]
        );
        $rule->update(apply: true);

        # Check that the previous rule is no longer present in the pfctl rules but the updated rule is
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $type = $rule->type->value;
        $if = "\${$rule->interface->get_related_model()->first()->descr->value}";
        $iptype = $rule->ipprotocol->value;
        $proto = $rule->protocol->value;
        $src = $rule->source->value;
        $srcport = $rule->source_port->value;
        $dst = $rule->destination->value;
        $dstport = $rule->destination_port->value;
        $descr = $rule->descr->value;
        $rid = $rule->tracker->value;
        $pfctl_rule_updated = "$type  in  quick  on $if $iptype proto $proto  from $src port $srcport to $dst port ".
            "$dstport ridentifier $rid label \"USER_RULE: $descr\" label \"id:$rid\"";
        $this->assert_str_does_not_contain($pfctl_rules, $pfctl_rule_created);
        $this->assert_str_contains($pfctl_rules, $pfctl_rule_updated);

        # Delete the rule and ensure it is no longer present in pfctl
        $rule->delete(apply: true);
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain($pfctl_rules, $pfctl_rule_updated);
    }

    /**
     * Checks that firewall rules created with a specific gateway assigned are properly set in pfctl
     */
    public function test_rule_with_gateway() {
        # Create a routing gatewy to test with
        $gateway = new RoutingGateway(
            data: [
                "name" => "TEST_GW",
                "interface" => "lan",
                "ipprotocol" => "inet",
                "gateway" => "192.168.1.50"
            ]
        );
        $gateway->create();

        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "source" => "any",
                "destination" => "any",
                "gateway" => $gateway->name->value
            ],
            async: false
        );
        $rule->create(apply: true);

        # Check that this rule is present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $type = $rule->type->value;
        $if = "\${$rule->interface->get_related_model()->first()->descr->value}";
        $iptype = $rule->ipprotocol->value;
        $src = $rule->source->value;
        $dst = $rule->destination->value;
        $rid = $rule->tracker->value;
        $gw = $rule->gateway->value;
        $pfctl_rule_created = "pass  in  quick  on $if  \$GW$gw $iptype from $src to $dst ridentifier $rid keep state label \"USER_RULE\" label \"id:$rid\" label \"gw:$gw\"";
        $this->assert_str_contains($pfctl_rules, $pfctl_rule_created);

        # Delete the firewall rule and gateway
        $rule->delete(apply: true);
        $gateway->delete();
    }

    /**
     * Checks that disabled firewall rules are not present in pfctl
     */
    public function test_disabled() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "disabled" => true,
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "source" => "any",
                "destination" => "any"
            ],
            async: false
        );
        $rule->create(apply: true);

        # Ensure the created rule's tracker ID is not present in the pfctl rules
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_does_not_contain(
            $pfctl_rules,
            $rule->tracker->value
        );

        # Delete the firewall rule
        $rule->delete(apply: true);
    }

    /**
     * Checks that the `statetype` is properly set in the pfctl rule
     */
    public function test_statetype() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "source" => "any",
                "destination" => "any",
                "statetype" => "sloppy state"
            ],
            async: false
        );
        $rule->create(apply: true);

        # Ensure the pfctl rule with this rule object's tracker is set to sloppy
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $pfctl_rule = "ridentifier {$rule->tracker->value} keep state ( sloppy  )";
        $this->assert_str_contains($pfctl_rules, $pfctl_rule);

        # Delete the firewall rule
        $rule->delete(apply: true);
    }

    /**
     * Checks that firewall rules with `log` enabled have the log flag set in pfctl
     */
    public function test_log() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "source" => "any",
                "destination" => "any",
                "log" => true
            ],
            async: false
        );
        $rule->create(apply: true);

        # Ensure the log flag is set in the rule within pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $if = $rule->interface->get_related_model()->first()->descr->value;
        $type = $rule->type->value;
        $iptype = $rule->ipprotocol->value;
        $src = $rule->source->value;
        $dst = $rule->destination->value;
        $rid = $rule->tracker->value;
        $this->assert_str_contains(
            $pfctl_rules,
            "$type  in log  quick  on \$$if $iptype from $src to $dst ridentifier $rid"
        );

        # Delete the firewall rule
        $rule->delete(apply: true);
    }

    /**
     * Checks that firewall rules with `tcp_flags_any` enabled have the `any` flag set in pfctl
     */
    public function test_tcp_flags_any() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "any",
                "tcp_flags_any" => true
            ],
            async: false
        );
        $rule->create(apply: true);

        # Ensure the `any` tcp flag is set in the rule within pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_contains(
            $pfctl_rules,
            "ridentifier {$rule->tracker->value} flags any"
        );

        # Delete the firewall rule
        $rule->delete(apply: true);
    }

    /**
     * Checks that firewall rules with explicit TCP flags set are correctly added in pfctl
     */
    public function test_tcp_flags() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["lan"],
                "ipprotocol" => "inet",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "any",
                "tcp_flags_set" => ["fin", "syn", "ack"],
                "tcp_flags_out_of" => ["fin", "syn", "rst", "ack"]
            ],
            async: false
        );
        $rule->create(apply: true);
        
        # Ensure the `any` tcp flag is set in the rule within pfctl
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $this->assert_str_contains(
            $pfctl_rules,
            "ridentifier {$rule->tracker->value} flags FSA/FSRA"
        );

        # Delete the firewall rule
        $rule->delete(apply: true);
    }

    /**
     * Checks that firewall rules with `floating` enabled are correctly added in pfctl
     */
    public function test_floating() {
        # Create a firewall rule to test with
        $rule = new FirewallRule(
            data: [
                "type" => "pass",
                "interface" => ["wan", "lan"],
                "ipprotocol" => "inet",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "any",
                "floating" => true,
                "quick" => false,
                "direction" => "out"
            ],
            async: false
        );
        $rule->create(apply: true);

        # Ensure the floating rule is present in pfctl as configured above
        $pfctl_rules = file_get_contents("/tmp/rules.debug");
        $rid = $rule->tracker->value;
        $pfctl_rule = "pass  out  on {  em0  em1  } inet proto tcp  from any to any ridentifier $rid";
        $this->assert_str_contains(
            $pfctl_rules,
            $pfctl_rule
        );

        # Delete the firewall rule
        $rule->delete(apply: true);
    }
}
