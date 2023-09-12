<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\FirewallAlias;

class APIModelsFirewallAliasTestCase extends TestCase {
    /**
     * Checks that aliases with hostnames correctly populate a pfctl table
     */
    public function test_vlan_configured_in_ifconfig() {
        # Create an alias that includes dns.google as an alias item
        $test_alias = new FirewallAlias(
            representation_data: [
                "name" => "TEST_GOOGLE_DNS",
                "type" => "host",
                "address" => ["dns.google"]
            ]
        );
        $test_alias->create(apply: true);

        # Wait up to 30 seconds for the filter to reload and the table to be create
        foreach (range(0, 30) as $attempt) {
            # Check pfctl for the table
            $pfctl_output = shell_exec("pfctl -t TEST_GOOGLE_DNS -Ts");
            if ($pfctl_output) {
                break;
            }

            # Wait 1 second before trying again
            sleep(1);
        }

        # Ensure expected IPs were resolved and stored in a pfctl with the same name as our alias
        $this->assert_is_true(str_contains($pfctl_output, "8.8.8.8"));
        $this->assert_is_true(str_contains($pfctl_output, "8.8.4.4"));

        # Delete the alias
        $test_alias->delete(apply: true);
    }

    /**
     * Checks that prohibited alias names are rejected
     */
    public function test_reject_prohibited_alias_names() {
        # Ensure aliases starting with `pkg_` are rejected
        $this->assert_throws_response(
            response_id: "INVALID_FIREWALL_ALIAS_NAME",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    representation_data: ["name" => "pkg_anything", "type" => "host"]
                );
                $test_alias->validate();
            }
        );

        # Ensure aliases starting with `wan` are rejected
        $this->assert_throws_response(
            response_id: "INVALID_FIREWALL_ALIAS_NAME",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    representation_data: ["name" => "wan", "type" => "host"]
                );
                $test_alias->validate();
            }
        );

        # Ensure aliases with `lan` are rejected
        $this->assert_throws_response(
            response_id: "INVALID_FIREWALL_ALIAS_NAME",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    representation_data: ["name" => "lan", "type" => "host"]
                );
                $test_alias->validate();
            }
        );
    }
}
