<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\InterfaceField;
use RESTAPI\Fields\StringField;

/**
 * Defines a model that represents a DHCP lease.
 */
class DHCPServerLease extends Model
{
    public StringField $ip;
    public StringField $mac;
    public StringField $hostname;
    public InterfaceField $if;
    public StringField $starts;
    public StringField $ends;
    public StringField $active_status;
    public StringField $online_status;
    public StringField $descr;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->internal_callable = 'get_dhcp_leases';
        $this->many = true;

        # Set model fields
        $this->ip = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The IP address of the lease.'
        );
        $this->mac = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The MAC address of the lease.'
        );
        $this->hostname = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The hostname of the lease.'
        );
        $this->if = new InterfaceField(
            default: null,
            allow_null: true,
            help_text: 'The interface the lease was registered on.'
        );
        $this->starts = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The start time of the lease.'
        );
        $this->ends = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The end time of the lease.'
        );
        $this->active_status = new StringField(
            default: null,
            allow_null: true,
            internal_name: 'act',
            help_text: 'The active status of the lease.'
        );
        $this->online_status = new StringField(
            default: null,
            allow_null: true,
            internal_name: 'online',
            help_text: 'The online status of the lease.'
        );
        $this->descr = new StringField(
            default: null,
            allow_null: true,
            help_text: 'The description of the lease.'
        );


        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Obtains the DHCP leases from the system internally. This is the internal callable for the Model.
     * @return array An array of recent/current DHCP leases
     */
    protected function get_dhcp_leases(): array
    {
        return system_get_dhcpleases()["lease"];
    }
}