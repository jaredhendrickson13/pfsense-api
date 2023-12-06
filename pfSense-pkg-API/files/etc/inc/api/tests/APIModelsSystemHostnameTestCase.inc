<?php

namespace API\Tests;

use API\Core\Command;
use API\Core\TestCase;
use API\Models\SystemHostname;

class APIModelsSystemHostnameTestCase extends TestCase
{
    /**
     * Checks that the system hostname is configured after updating.
     */
    public function test_update() {
        # Create the SystemHostname model object to test with
        $hostname = new SystemHostname(async: false);

        # Remember the original hostname and domain
        $original_hostname = $hostname->hostname->value;
        $original_domain = $hostname->domain->value;

        # Update the system hostname to something different and ensure it is configured
        $hostname->hostname->value = "test";
        $hostname->domain->value = "example.com";
        $hostname->update();
        $hostname_cmd = new Command("hostname");
        $this->assert_str_contains($hostname_cmd->output, "test.example.com");

        # Restore the original hostname and domain and ensure it is configured
        $hostname->hostname->value = $original_hostname;
        $hostname->domain->value = $original_domain;
        $hostname->update();
        $hostname_cmd = new Command("hostname");
        $this->assert_str_contains($hostname_cmd->output, "$original_hostname.$original_domain");
    }
}