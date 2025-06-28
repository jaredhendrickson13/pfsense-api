<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\Model;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemStatus;

class APIModelsSystemStatusTestCase extends TestCase {
    /**
     * Checks that we can read the current system status and that fields are properly assigned the correct values.
     */
    public function test_read(): void {
        # Read the current system status and ensure fields are assigned valid values
        $system_status = new SystemStatus();
        $this->assert_equals($system_status->platform->value, system_identify_specific_platform()['descr']);
        $this->assert_equals($system_status->serial->value, system_get_serial());
        $this->assert_equals($system_status->netgate_id->value, system_get_uniqueid());
        $this->assert_is_not_empty($system_status->uptime->value);
        $this->assert_equals($system_status->cpu_model->value, get_single_sysctl('hw.model'));
        $this->assert_equals($system_status->kernel_pti->value, (bool) get_single_sysctl('vm.pmap.pti'));
        $this->assert_equals($system_status->mds_mitigation->value, get_single_sysctl('hw.mds_disable_state'));
        $this->assert_equals(count($system_status->cpu_load_avg->value), 3);
        $this->assert_equals($system_status->cpu_count->value, (int) get_single_sysctl('hw.ncpu'));
        $this->assert_is_greater_than_or_equal($system_status->cpu_usage->value, 0);
        $this->assert_is_greater_than_or_equal($system_status->mem_usage->value, 0);
        $this->assert_is_less_than_or_equal($system_status->mem_usage->value, 100);
        $this->assert_is_greater_than_or_equal($system_status->mbuf_usage->value, 0);
        $this->assert_is_less_than_or_equal($system_status->mbuf_usage->value, 100);
        $this->assert_is_greater_than_or_equal($system_status->swap_usage->value, 0);
        $this->assert_is_less_than_or_equal($system_status->swap_usage->value, 100);
        $this->assert_is_greater_than_or_equal($system_status->disk_usage->value, 0);
        $this->assert_is_less_than_or_equal($system_status->disk_usage->value, 100);
        $this->assert_equals(
            $system_status->bios_vendor->value,
            (new Command(command: '/bin/kenv -q smbios.bios.vendor 2>/dev/null', trim_whitespace: true))->output,
        );
        $this->assert_equals(
            $system_status->bios_version->value,
            (new Command(command: '/bin/kenv -q smbios.bios.version 2>/dev/null', trim_whitespace: true))->output,
        );
        $this->assert_equals(
            $system_status->bios_date->value,
            (new Command(command: '/bin/kenv -q smbios.bios.reldate 2>/dev/null', trim_whitespace: true))->output,
        );
    }

    /**
     * Checks that the load average can can be read correctly despite the system's assign language. Normally,
     * the load average format can change based on the systems assigned language and cause issues parsing the
     * value into an array. This is a regression test for #716.
     */
    public function test_language_does_not_change_cpu_load_avg(): void {
        # Change the language to German
        Model::set_config('system/language', 'de_DE');
        set_language();

        # Ensure we can read the SystemStatus cpu_load_average without an error
        $this->assert_does_not_throw(
            callable: function () {
                $system_status = new SystemStatus();
                $this->assert_equals(count($system_status->cpu_load_avg->value), 3);
            },
        );

        # Reset the language to English and check again
        Model::set_config('system/language', 'en_US');
        set_language();
        $this->assert_does_not_throw(
            callable: function () {
                $system_status = new SystemStatus();
                $this->assert_equals(count($system_status->cpu_load_avg->value), 3);
            },
        );
    }
}
