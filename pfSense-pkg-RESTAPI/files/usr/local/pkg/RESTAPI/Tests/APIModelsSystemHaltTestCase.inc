<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemHalt;

class APIModelsSystemHaltTestCase extends TestCase {
    /**
     * Check that we can initiate a system halt.
     */
    public function test_initiate_shutdown(): void {
        $this->assert_does_not_throw(
            callable: function () {
                (new SystemHalt(dry_run: true))->create();
            },
        );
    }
}
