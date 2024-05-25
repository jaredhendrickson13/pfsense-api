<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallStatesSize;

class APIModelsFirewallStatesSizeTestCase extends TestCase {
    /**
     * Ensure the `from_internal_defaultmaximumstates()` obtains the correct default state table size.
     */
    public function test_from_internal_defaultmaximumstates() {
        # Create a FirewallStatesSize object to test with
        $states_size = new FirewallStatesSize();

        # Ensure the default size is accurate
        $this->assert_equals($states_size->defaultmaximumstates->value, pfsense_default_state_size());
    }

    /**
     * Ensure setting the `maximumstates` field actually sets the states limit in pfctl
     */
    public function test_maximumstates_set_in_pfctl() {
        # Create a FirewallStatesSize object to test with
        $states_size = new FirewallStatesSize(async: false);
        $states_size->maximumstates->value = 500;
        $states_size->update(apply: true);
        sleep(1);

        # Check the hard states limit in pfctl and ensure it matches the value we set
        $pfctl_sm = new Command('pfctl -sm', trim_whitespace: true);
        $this->assert_str_contains($pfctl_sm->output, 'states hard limit 500');

        # Set the states limit back to the default
        $states_size->maximumstates->value = $states_size->defaultmaximumstates->value;
        $states_size->update(apply: true);
    }
}
