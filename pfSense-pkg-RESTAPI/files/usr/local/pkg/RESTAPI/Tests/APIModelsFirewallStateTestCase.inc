<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallState;

class APIModelsFirewallStateTestCase extends TestCase {
    /**
     * Checks that states are generated when traffic is passed through the firewall, that we can read the states
     * correctly, and that we can successfully kill the firewall state using the delete() method.
     */
    public function test_state_read_and_kill(): void {
        # Send a single ping to a test IP to register a new firewall state
        new Command('ping -t1 -c1 127.0.0.50');

        # Ensure we can query and find the newly registered firewall state
        $state_q = FirewallState::query(destination__startswith: '127.0.0.50', protocol: 'icmp');

        # Ensure the state was found
        $this->assert_is_true($state_q->exists());
        $state = $state_q->first();

        # Ensure the values assigned to the state are expected
        $this->assert_is_not_empty($state->interface->value);
        $this->assert_equals($state->protocol->value, 'icmp');
        $this->assert_equals($state->direction->value, 'out');
        $this->assert_is_not_empty($state->age->value);
        $this->assert_is_not_empty($state->expires_in->value);
        $this->assert_equals($state->packets_total->value, 1);
        $this->assert_equals($state->packets_in->value, 1);
        $this->assert_equals($state->packets_out->value, 0);
        $this->assert_is_greater_than($state->bytes_total->value, 0);
        $this->assert_is_greater_than($state->bytes_in->value, 0);
        $this->assert_equals($state->bytes_out->value, 0);

        # Kill the firewall state
        $state->delete();

        # Ensure the state is no longer present
        $this->assert_is_false(FirewallState::query(destination__startswith: '127.0.0.50')->exists());
    }
}
