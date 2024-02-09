<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\WireGuardSettingsDispatcher;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\IPAddressValidator;

class WireGuardTunnelAddress extends Model
{
    public StringField $address;
    public IntegerField $mask;
    public StringField $descr;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->many = true;
        $this->parent_model_class = 'WireGuardTunnel';
        $this->config_path = "addresses/row";
        $this->verbose_name = 'WireGuard Tunnel Address';
        $this->packages = ['pfSense-pkg-WireGuard'];
        $this->package_includes = ['wireguard/includes/wg_service.inc', 'wireguard/includes/wg.inc'];

        # Set model fields
        $this->address = new StringField(
            required: true,
            validators: [new IPAddressValidator()],
            help_text: "The IPv4 or IPv6 address for this WireGuard tunnel."
        );
        $this->mask = new IntegerField(
            required: true,
            minimum: 1,
            maximum: 128,
            help_text: "The subnet mask for this WireGuard tunnel."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A description for this WireGuard tunnel address entry."
        );

        parent::__construct($id, $parent_id, $data, $options);
    }

    /**
     * Serializes the parent WireGuardTunnel before applying changes.
     */
    public function pre_apply(): void
    {
        $this->parent_model->pre_apply();
    }

    /**
     * Applies pending changes to WireGuard tunnels.
     */
    public function apply(): void
    {
        (new WireGuardSettingsDispatcher(async: $this->async))->spawn_process();
    }
}