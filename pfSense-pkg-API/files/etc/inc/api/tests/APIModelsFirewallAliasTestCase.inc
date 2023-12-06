<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\FirewallAlias;

class APIModelsFirewallAliasTestCase extends TestCase {
    /**
     * Checks that aliases with hostnames correctly populate a pfctl table
     */
    public function test_fqdn_alias_populates_pfctl_table() {
        # Create an alias that includes dns.google as an alias item
        $test_alias = new FirewallAlias(
            data: [
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
        $this->assert_str_contains($pfctl_output, "8.8.8.8");
        $this->assert_str_contains($pfctl_output, "8.8.4.4");

        # Delete the alias
        $test_alias->delete(apply: true);
    }

    /**
     * Checks that prohibited alias names are rejected
     */
    public function test_reject_prohibited_alias_names() {
        # Ensure aliases starting with `pkg_` are rejected
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_CANNOT_START_WITH_PKG",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "pkg_anything", "type" => "host"]
                );
                $test_alias->validate();
            }
        );

        # Ensure reserved table names cannot be used
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_RESERVED_NAME_USED",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "sshguard", "type" => "host"]
                );
                $test_alias->validate();
            }
        );

        # Ensure reserved table names cannot be used
        $this->assert_throws_response(
            response_id: "FILTER_NAME_VALIDATOR_RESERVED_NAME_USED",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "IPsec", "type" => "host"]
                );
                $test_alias->validate();
            }
        );
    }

    /**
     * Checks that address entries for host aliases must be an IP, hostname or alias
     */
    public function test_host_alias_address_validation() {
        # Ensure an error is thrown if address is not a host
        $this->assert_throws_response(
            response_id: "INVALID_HOST_ALIAS_ADDRESS",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "test_alias", "type" => "host", "address" => ["!@# Not a host!"]]
                );
                $test_alias->validate();
            }
        );
    }

    /**
     * Checks that address entries for network aliases must be a CIDR, hostname or alias
     */
    public function test_network_alias_address_validation() {
        $this->assert_throws_response(
            response_id: "INVALID_NETWORK_ALIAS_ADDRESS",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "test_alias", "type" => "network", "address" => ["!@# Not a network!"]]
                );
                $test_alias->validate();
            }
        );
    }

    /**
     * Checks that address entries for port aliases must be a port number or alias
     */
    public function test_port_alias_address_validation() {
        $this->assert_throws_response(
            response_id: "INVALID_PORT_ALIAS_ADDRESS",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: ["name" => "test_alias", "type" => "port", "address" => ["!@# Not a port!"]]
                );
                $test_alias->validate();
            }
        );
    }

    /**
     * Checks that there cannot be more detail entries than address entries.
     */
    public function test_cannot_have_more_details_than_addresses() {
        $this->assert_throws_response(
            response_id: "TOO_MANY_ALIAS_DETAILS",
            code: 400,
            callable: function() {
                $test_alias = new FirewallAlias(
                    data: [
                        "name" => "test_alias",
                        "type" => "port",
                        "address" => ["1", "2", "3", "4"],
                        "detail" => ["1", "2", "3", "4", "5"]
                    ]
                );
                $test_alias->validate();
            }
        );
    }
}
