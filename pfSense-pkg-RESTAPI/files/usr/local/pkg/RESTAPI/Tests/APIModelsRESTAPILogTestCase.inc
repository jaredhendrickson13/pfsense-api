<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPILog;

class APIModelsRESTAPILogTestCase extends TestCase {
    /**
     * Checks if we can correctly read the restapi logs.
     */
    public function test_read(): void {
        $dhcp_logs = RESTAPILog::read_all(limit: 10);
        foreach ($dhcp_logs->model_objects as $dhcp_log) {
            $this->assert_is_not_empty($dhcp_log->text->value);
        }
    }
}
