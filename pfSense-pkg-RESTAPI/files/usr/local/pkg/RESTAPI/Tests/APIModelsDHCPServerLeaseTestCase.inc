<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServerLease;
use RESTAPI\Models\DHCPServerStaticMapping;

class APIModelsDHCPServerLeaseTestCase extends TestCase {
    /**
     * Checks that we can correct read DHCP leases
     */
    public function test_read(): void {
        # Create a DHCP server static mapping to populate a DHCP lease
        $static_mapping = new DHCPServerStaticMapping(
            parent_id: 'lan',
            mac: '00:00:00:00:00:00',
            ipaddr: '192.168.1.2',
            hostname: 'test-hostname',
            descr: 'test-description',
            async: false,
        );
        $static_mapping->create(apply: true);

        # Read current DHCP leases and ensure our static mapping is present
        $lease_q = DHCPServerLease::query(mac: '00:00:00:00:00:00');
        $this->assert_is_true($lease_q->exists());
        $this->assert_equals($lease_q->first()->ip->value, $static_mapping->ipaddr->value);
        $this->assert_equals($lease_q->first()->mac->value, $static_mapping->mac->value);
        $this->assert_equals($lease_q->first()->hostname->value, $static_mapping->hostname->value);
        $this->assert_equals($lease_q->first()->descr->value, $static_mapping->descr->value);
    }
}
