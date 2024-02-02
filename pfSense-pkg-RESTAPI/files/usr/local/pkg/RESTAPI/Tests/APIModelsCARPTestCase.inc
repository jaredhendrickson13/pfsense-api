<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\CARP;
use RESTAPI\Models\VirtualIP;

class APIModelsCARPTestCase extends TestCase
{
    /**
     * Checks that CARP is not enabled when no CARP virtual IPs exist, but is then enabled after the first CARP
     * virtual IP is created.
     */
    public function test_enable()
    {
        # Create CARP status and CARP VIP objects to test with
        $carp_status = new CARP(async: false);
        $carp_vip = new VirtualIP(
            data: [
                'mode' => 'carp',
                'interface' => 'lan',
                'subnet' => '127.1.2.3',
                'subnet_bits' => 32,
                'password' => 'testpasswd',
                'vhid' => 5,
            ],
        );
        $carp_vip->create(apply: true);

        # Ensure CARP is now enabled and that the CARP virtual IP is present in ifconfig
        $carp_status = new CARP(async: false);
        $this->assert_is_true($carp_status->enable->value);
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains($ifconfig->output, $carp_vip->subnet->value);

        # Disable CARP and ensure the CARP VIP is no longer present in ifconfig
        $carp_status->enable->value = false;
        $carp_status->update();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_does_not_contain($ifconfig->output, $carp_vip->subnet->value);

        # Re-enable CARP and ensure the CARP VIP is once again present in ifconfig
        $carp_status->enable->value = true;
        $carp_status->update();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains($ifconfig->output, $carp_vip->subnet->value);

        # Delete the virtual IP used for testing
        $carp_vip->delete(apply: true);
    }

    /**
     * Checks that `maintenance_mode` properly marks CARP members as backup when set.
     */
    public function test_maintenance_mode()
    {
        # Create CARP status and CARP VIP objects to test with
        $carp_status = new CARP(async: false);
        $carp_vip = new VirtualIP(
            data: [
                'mode' => 'carp',
                'interface' => 'lan',
                'subnet' => '127.1.2.3',
                'subnet_bits' => 32,
                'password' => 'testpasswd',
                'vhid' => 5,
                'advbase' => 1,
                'advskew' => 1,
            ],
        );
        $carp_vip->create(apply: true);

        # Ensure the CARP VIP is present in ifconfig with the proper `advskew` set.
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains(
            $ifconfig->output,
            "vhid {$carp_vip->vhid->value} advbase {$carp_vip->advbase->value} advskew {$carp_vip->advskew->value}",
        );

        # Enable CARP maintenance mode and ensure the advskew is changed to 254 (the max)
        $carp_status->maintenance_mode->value = true;
        $carp_status->update();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains(
            $ifconfig->output,
            "vhid {$carp_vip->vhid->value} advbase {$carp_vip->advbase->value} advskew {$carp_vip->advskew->maximum}",
        );

        # Disable CARP maintenance mode and ensure the advskew is returned to the configured value
        $carp_status->maintenance_mode->value = false;
        $carp_status->update();
        $ifconfig = new Command('ifconfig');
        $this->assert_str_contains(
            $ifconfig->output,
            "vhid {$carp_vip->vhid->value} advbase {$carp_vip->advbase->value} advskew {$carp_vip->advskew->value}",
        );

        # Delete the virtual IP
        $carp_vip->delete(apply: true);
    }
}
