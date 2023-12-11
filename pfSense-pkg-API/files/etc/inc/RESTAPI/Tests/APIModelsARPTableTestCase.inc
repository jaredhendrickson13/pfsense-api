<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\ARPTable;

class APIModelsARPTableTestCase extends TestCase
{
    /**
     * Checks that ARP table entries are correctly deleted from the ARP table when the `delete()` method runs.
     */
    public function test_arp_table_entry_deletion() {
        # Create a static ARP entry to use for testing
        new Command("arp -s 192.168.1.2 11:22:33:44:aa:bb");

        # Ensure the ARP entry is correctly found in the ARP table now
        $arp_query = ARPTable::query(["mac_address" => "11:22:33:44:aa:bb"]);
        $this->assert_is_true($arp_query->exists());

        # Delete the ARP entry then ensure it no longer exists in the ARP table
        $arp_query->first()->delete();
        $arp_query = ARPTable::query(["mac_address" => "11:22:33:44:aa:bb"]);
        $this->assert_is_false($arp_query->exists());
    }
}
