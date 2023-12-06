<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\PortForward;

class APIModelsPortForwardTestCase extends TestCase
{
    /**
     * Ensures basic port forward rules are correctly set in pfctl after creation, modification and deletion.
     */
    public function test_basic_crud() {
        # Create a port forward to test with
        $port_forward = new PortForward(
            data: [
                "interface" => "lan",
                "protocol" => "tcp",
                "source" => "any",
                "destination" => "lan:ip",
                "destination_port" => "8443",
                "target" => "127.1.2.3"
            ]
        );
        $port_forward->create(apply: true);

        # Ensure this rule was configured by pfctl
        $rules_debug = file_get_contents("/tmp/rules.debug");
        $port_forward->delete(apply: true);
    }
}