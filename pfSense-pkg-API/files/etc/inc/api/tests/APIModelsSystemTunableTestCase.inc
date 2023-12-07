<?php

namespace API\Tests;

use API\Core\Command;
use API\Core\TestCase;
use API\Models\SystemTunable;

class APIModelsSystemTunableTestCase extends TestCase
{
    /**
     * Ensures that the tunable is correctly set in sysctl after creating, updating and delete SystemTunable objects.
     */
    public function test_crud() {
        # Sets the sysctl OID value to use for testing
        $test_oid = "net.link.bridge.pfil_bridge";
        
        # First, check the default value for the OID
        $oid_default = new Command("sysctl $test_oid");
        
        # Create a custom system tunable for our OID and ensure it is actually set in sysctl
        $tunable = new SystemTunable(data: ["tunable" => $test_oid, "value" => "30"]);
        $tunable->create(apply: true);
        $oid_value = new Command("sysctl $test_oid");
        $this->assert_str_contains($oid_value->output, "$test_oid: 30");

        # Create the custom system tunable for our OID and ensure its new value set in sysctl
        $tunable->value->value = "10";
        $tunable->update(apply: true);
        $oid_value = new Command("sysctl $test_oid");
        $this->assert_str_contains($oid_value->output, "$test_oid: 10");

        # Delete the custom tunable and ensure the OID value returns to its original value
        $tunable->delete(apply: true);
        $oid_value = new Command("sysctl $test_oid");
        $this->assert_equals($oid_value->output, $oid_default->output);
    }
}
