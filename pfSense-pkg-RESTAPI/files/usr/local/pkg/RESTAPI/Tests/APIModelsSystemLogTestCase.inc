<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemLog;

class APIModelsSystemLogTestCase extends TestCase
{
    /**
     * Checks if we can correctly read the system logs.
     */
    public function test_read(): void
    {
        $system_logs = SystemLog::read_all(limit: 10);
        foreach ($system_logs->model_objects as $system_log) {
            $this->assert_is_not_empty($system_log->text->value);
        }
    }
}