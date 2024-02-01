<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Command;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\FloatField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;

class SystemStatus extends Model
{
    public StringField $system_platform;
    public StringField $system_serial;
    public StringField $system_netgate_id;
    public StringField $bios_vendor;
    public StringField $bios_version;
    public StringField $bios_date;
    public BooleanField $kernel_pti;
    public StringField $mds_mitigation;
    public FloatField $temp_c;
    public FloatField $temp_f;
    public StringField $cpu_model;
    public FloatField $cpu_load_avg;
    public IntegerField $cpu_count;
    public FloatField $cpu_usage;
    public FloatField $mbuf_usage;
    public FloatField $mem_usage;
    public FloatField $swap_usage;
    public FloatField $disk_usage;

    public function __construct(mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->internal_callable = "get_system_status_internal";

        # Set model fields
        $this->system_platform = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The verbose name of this system's platform."
        );
        $this->system_serial = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The system's unique serial number/ID."
        );
        $this->system_netgate_id = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The unique ID issued for this pfSense instance by Netgate."
        );
        $this->bios_vendor = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The name of the BIOS vendor."
        );
        $this->bios_version = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The BIOS version installed on this system."
        );
        $this->bios_date = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The build date of the BIOS version."
        );
        $this->kernel_pti = new BooleanField(
            default: false,
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: "Indicates whether kernel PTI is enabled or not."
        );
        $this->mds_mitigation = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "Indicates whether MDS mitigation is enabled or not."
        );
        $this->temp_c = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current system temperature in celsius."
        );
        $this->temp_f = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current system temperature in fahrenheit."
        );
        $this->cpu_model = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The model of CPU installed in this system and other CPU info."
        );
        $this->cpu_load_avg = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            many: true,
            help_text: "The CPU load averages. The first value represents the load average for the last minute, the ".
                "second value represents the load average for the last 5 minutes, and the third value represents the ".
                "load average for the last 15 minutes."
        );
        $this->cpu_count = new IntegerField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The total number of CPUs cores available on this system."
        );
        $this->cpu_usage = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current CPU usage as a decimal percentage."
        );
        $this->mem_usage = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current memory usage as a decimal percentage."
        );
        $this->mbuf_usage = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current MBUF usage as a decimal percentage."
        );
        $this->disk_usage = new FloatField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: "The current disk usage as a decimal percentage."
        );

        parent::__construct(...$options, data: $data);
    }

    /**
     * Obtains the internal values for the systems status. This method is the internally callable and is used to
     * populate the Field values for this Model.
     * @return array An array of internal system status values.
     */
    protected function get_system_status_internal(): array {
        return [
            "system_platform" => system_identify_specific_platform()["descr"],
            "system_serial" => system_get_serial(),
            "system_netgate_id" => system_get_uniqueid(),
            "bios_vendor" => (
                new Command(
                    command: '/bin/kenv -q smbios.bios.vendor 2>/dev/null',
                    trim_whitespace: true
                )
            )->output,
            "bios_version" => (
                new Command(
                    command: '/bin/kenv -q smbios.bios.version 2>/dev/null',
                    trim_whitespace: true
                )
            )->output,
            "bios_date" => (
                new Command(
                    command: '/bin/kenv -q smbios.bios.reldate 2>/dev/null',
                    trim_whitespace: true
                )
            )->output,
            "cpu_model" => get_single_sysctl("hw.model"),
            "kernel_pti" => boolval(get_single_sysctl('vm.pmap.pti')),
            "mds_mitigation" => get_single_sysctl('hw.mds_disable_state'),
            "temp_c" => (get_temp()) ?: null,
            "temp_f" => (get_temp()) ? floatval(get_temp()) * 1.8 + 32 : null,
            "load_avg" => get_load_average(),
            "cpu_count" => get_single_sysctl("hw.ncpu"),
            "mbuf_usage" => $this->__get_mbuf_usage(),
            "mem_usage" => (!is_null(mem_usage())) ? APITools\float_percent(mem_usage()) : null,
            "swap_usage" => (!is_null(swap_usage())) ? APITools\float_percent(swap_usage()) : null,
            "disk_usage" => $this->__get_filesystem_usage()
        ];
    }
}