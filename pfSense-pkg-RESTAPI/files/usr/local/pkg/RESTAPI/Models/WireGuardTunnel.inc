<?php

namespace RESTAPI\Models;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\WireGuardSettingsDispatcher;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ConflictError;
use RESTAPI\Responses\ValidationError;

class WireGuardTunnel extends Model {
    public StringField $name;
    public BooleanField $enabled;
    public StringField $descr;
    public PortField $listenport;
    public StringField $publickey;
    public StringField $privatekey;
    public IntegerField $mtu;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->many = true;
        $this->config_path = 'installedpackages/wireguard/tunnels/item';
        $this->verbose_name = 'WireGuard Tunnel';
        $this->subsystem = 'wireguard';
        $this->packages = ['pfSense-pkg-WireGuard'];
        $this->package_includes = ['wireguard/includes/wg_service.inc', 'wireguard/includes/wg.inc'];

        # Set model fields
        $this->name = new StringField(
            unique: true,
            default: 'tun_wg0', // TODO: change this to a callable that gets the next available wg interface name
            editable: false,
            read_only: true,
            help_text: 'The name of the WireGuard interface. This value is automatically assigned by the system and ' .
                'cannot be changed.',
        );
        $this->enabled = new BooleanField(
            default: true,
            indicates_true: 'yes',
            indicates_false: 'no',
            help_text: 'Enables or disables this tunnels and any associated peers.',
        );
        $this->listenport = new PortField(
            default: '51820',
            allow_alias: false,
            allow_range: false,
            help_text: 'The port WireGuard will listen on for this tunnel.',
        );
        $this->publickey = new StringField(
            required: true,
            help_text: 'The public key for this tunnel. This value and `privatekey` must be a matching keypair.',
        );
        $this->privatekey = new StringField(
            required: true,
            write_only: true,
            help_text: 'The private key for this tunnel. This value and `publickey` must be a matching keypair.',
        );
        $this->mtu = new IntegerField(
            default: 1420,
            help_text: 'The MTU for this WireGuard tunnel interface. This value is ignored if this tunnel is ' .
                'assigned as a pfSense interface.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Adds extra validation and processing to this entire Model object.
     */
    public function validate_extra(): void {
        # Ensure the `publickey` field is always assigned the public key for `privatekey` value
        $this->publickey->value = wg_gen_publickey($this->privatekey->value);
    }

    /**
     * Adds extra validation to the `enabled` field.
     * @param bool $enabled The incoming value to be validated.
     * @return bool The validated value to be assigned.
     * @throws ConflictError When $enabled is set to false but this tunnel is assigned to an active interface.
     */
    public function validate_enabled(bool $enabled): bool {
        # Do not allow the tunnel to be disabled if it is in use by an existing pfSense interface
        if (!$enabled and NetworkInterface::query(if: $this->name->value)->exists()) {
            throw new ConflictError(
                message: "Field `enabled` cannot be `false` because this $this->verbose_name is in use by an " .
                    'existing interface object.',
                response_id: 'WIREGUARD_TUNNEL_CANNOT_BE_DISABLED_WHILE_IN_USE_BY_INTERFACE',
            );
        }

        return $enabled;
    }

    /**
     * Adds extra validation to the `privatekey` field.
     * @param string $privatekey The incoming value to validated.
     * @return string The validated value to be assigned.
     * @throws ValidationError When $privatekey is not a valid WireGuard private key
     */
    public function validate_privatekey(string $privatekey): string {
        # Do not allow this value if it is not a valid WireGuard private key.
        if (!wg_is_valid_key($privatekey)) {
            throw new ValidationError(
                message: 'Field `privatekey` must be a valid WireGuard private key.',
                response_id: 'WIREGUARD_TUNNEL_INVALID_PRIVATE_KEY',
            );
        }

        # Ensure this key is always clamped
        return wg_is_key_clamped($privatekey) ? $privatekey : wg_clamp_key($privatekey);
    }

    /**
     * Serializes changes to this tunnel before applying.
     */
    public function pre_apply(): void {
        wg_apply_list_add('tunnels', [$this->name->value]);
    }

    /**
     * Applies pending changes to WireGuard tunnels
     */
    public function apply(): void {
        (new WireGuardSettingsDispatcher(async: $this->async))->spawn_process();
    }
}
