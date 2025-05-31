<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\OpenVPNLog;

class APIModelsOpenVPNLogTestCase extends TestCase {
    /**
     * Checks if we can correctly read the openvpn logs.
     */
    public function test_read(): void {
        new Command('echo testlog', redirect: '>> /var/log/openvpn.log');
        $openvpn_logs = OpenVPNLog::read_all(limit: 5);
        foreach ($openvpn_logs->model_objects as $openvpn_log) {
            $this->assert_is_not_empty($openvpn_log->text->value);
        }
    }
}
