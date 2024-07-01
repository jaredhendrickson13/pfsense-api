<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPLog;

class APIModelsDHCPLogTestCase extends TestCase {
    /**
     * Checks if we can correctly read the dhcp logs.
     */
    public function test_read(): void {
        $dhcp_logs = DHCPLog::read_all(limit: 10);
        foreach ($dhcp_logs->model_objects as $dhcp_log) {
            $this->assert_is_not_empty($dhcp_log->text->value);
        }
    }
}
