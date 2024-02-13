<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\IPAddressValidator;

class WireGuardPeer extends Model
{
    public BooleanField $enabled;
    public ForeignModelField $tun;
    public StringField $endpoint;
    public PortField $port;
    public StringField $descr;
    public IntegerField $persistentkeepalive;
    public StringField $publickey;
    public StringField $presharedkey;
    public NestedModelField $allowedips;
    
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->many = true;
        $this->config_path = 'installedpackages/wireguard/peers/item';
        $this->verbose_name = 'WireGuard Peer';
        $this->subsystem = 'wireguard';
        $this->packages = ['pfSense-pkg-WireGuard'];
        $this->package_includes = ['wireguard/includes/wg_service.inc', 'wireguard/includes/wg.inc'];
        
        # Set model fields
        $this->enabled = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "no",
            help_text: "Enables or disables this WireGuard peer."
        );
        $this->tun = new ForeignModelField(
            model_name: "WireGuardTunnel",
            model_field: "name",
            allowed_keywords: ["unassigned"],
            default: "unassigned",
            help_text: "The WireGuard tunnel for this peer."
        );
        $this->endpoint = new StringField(
            default: null,
            allow_null: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_fqdn: true)],
            help_text: "The IP address or hostname of the remote peer. Set to `null` to make this a dynamic endpoint."
        );
        $this->port = new PortField(
            default: "51820",
            conditions: ["!endpoint" => null],
            help_text: "The port used by the remote peer."
        );
        $this->persistentkeepalive = new IntegerField(
            default: null,
            allow_null: true,
            help_text: "The interval (in seconds) for Keep Alive packets sent to this peer. Set to `null` to disable."
        );
        $this->publickey = new StringField(
            required: true,
            help_text: "The public key for this peer."
        );
        $this->presharedkey = new StringField(
            default: "",
            allow_null: true,
            help_text: "The pre-shared key for this tunnel."
        );
        
        parent::__construct($id, $parent_id, $data, ...$options);
    }
}
