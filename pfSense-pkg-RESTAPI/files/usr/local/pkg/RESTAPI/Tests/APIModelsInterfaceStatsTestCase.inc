<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\InterfaceStats;

class APIModelsInterfaceStatsTestCase extends TestCase {
    /**
     * Checks that we can correct read interface statistics
     */
    public function test_read(): void {
        # Create a interface statistics to populate a interface
        $interface_stats = InterfaceStats::query(name: 'wan');
        $this->assert_is_true($interface_stats->exists());
        $this->assert_is_not_empty($interface_stats->first()->name->value);
        $this->assert_is_not_empty($interface_stats->first()->descr->value);
        $this->assert_is_not_empty($interface_stats->first()->hwif->value);
        $this->assert_is_not_empty($interface_stats->first()->macaddr->value);
        $this->assert_is_not_empty($interface_stats->first()->mtu->value);
        $this->assert_is_not_empty($interface_stats->first()->status->value);
        $this->assert_is_not_empty($interface_stats->first()->ipaddr->value);
        $this->assert_is_not_empty($interface_stats->first()->subnet->value);
        $this->assert_is_not_empty($interface_stats->first()->media->value);
        $this->assert_is_not_empty($interface_stats->first()->gateway->value);
    }
}
