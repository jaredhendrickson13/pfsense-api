<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemReboot;

class APIModelsSystemRebootTestCase extends TestCase
{
    /**
     * Check that we can initiate a system reboot.
     */
    public function test_initiate_reboot(): void
    {
        $this->assert_does_not_throw(
            callable: function () {
                (new SystemReboot(dry_run: true))->create();
            },
        );
    }
}
