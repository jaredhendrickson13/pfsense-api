<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServerLease;
use RESTAPI\Models\DHCPServerStaticMapping;

class APIModelsDHCPServerLeaseTestCase extends TestCase {
    /**
     * Checks that we can correct read and delete DHCP leases
     */
    public function test_crud(): void {
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

        # Ensure we can delete the DHCP lease
        $lease_q->delete();

        # Ensure we can delete all leases
        $this->assert_is_greater_than_or_equal(DHCPServerLease::delete_all()->count(), 1);

        # Delete the static mapping
        $static_mapping->delete(apply: true);
    }

    /**
     * Checks that the isc_remove_lease method correctly removes a DHCP lease from the ISC leases file
     */
    public function test_isc_remove_lease(): void {
        # Mock some lease data to use for this test
        $mock_lease_data =
            'lease 127.0.0.1 {' .
            PHP_EOL .
            '   doesnt matter whats here' .
            PHP_EOL .
            '}' .
            PHP_EOL .
            'lease 127.0.0.2 {' .
            PHP_EOL .
            '   doesnt matter whats here' .
            PHP_EOL .
            '}' .
            PHP_EOL .
            'lease 127.0.0.3 {' .
            PHP_EOL .
            '   doesnt matter whats here' .
            PHP_EOL .
            '}';

        $this->assert_equals(
            DHCPServerLease::isc_remove_lease($mock_lease_data, ip: '127.0.0.2'),
            'lease 127.0.0.1 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}' .
                PHP_EOL .
                'lease 127.0.0.3 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}',
        );
        $this->assert_equals(
            DHCPServerLease::isc_remove_lease($mock_lease_data, ip: '127.0.0.1'),
            'lease 127.0.0.2 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}' .
                PHP_EOL .
                'lease 127.0.0.3 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}',
        );
        $this->assert_equals(
            DHCPServerLease::isc_remove_lease($mock_lease_data, ip: '127.0.0.3'),
            'lease 127.0.0.1 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}' .
                PHP_EOL .
                'lease 127.0.0.2 {' .
                PHP_EOL .
                '   doesnt matter whats here' .
                PHP_EOL .
                '}',
        );
    }
}
