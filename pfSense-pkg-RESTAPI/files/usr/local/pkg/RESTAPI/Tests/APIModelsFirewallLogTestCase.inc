<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallLog;

class APIModelsFirewallLogTestCase extends TestCase
{
    /**
     * Checks if we can correctly read the firewall logs.
     */
    public function test_read(): void
    {
        $firewall_logs = FirewallLog::read_all(limit: 5);
        foreach ($firewall_logs->model_objects as $firewall_log) {
            $this->assert_is_not_empty($firewall_log->text->value);
        }
    }
}