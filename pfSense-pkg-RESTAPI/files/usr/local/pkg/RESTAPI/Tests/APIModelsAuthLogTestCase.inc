<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\AuthLog;

class APIModelsAuthLogTestCase extends TestCase {
    /**
     * Checks if we can correctly read the auth logs.
     */
    public function test_read(): void {
        $auth_logs = AuthLog::read_all(limit: 5);
        foreach ($auth_logs->model_objects as $auth_log) {
            $this->assert_is_not_empty($auth_log->text->value);
        }
    }
}
