<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\FirewallRule;

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
}
