<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\IntegerField;
use API\Fields\StringField;
use API\Responses\ServerError;
use API\Responses\ValidationError;
use API\Validators\FilterNameValidator;
use API\Validators\GatewayValidator;
use API\Validators\IPAddressValidator;
use API\Validators\MACAddressValidator;

/**
 * A Model that allows us to read, create, update and delete network interfaces. Note: PHP does not allow us to name a
 * class `Interface` so we must use `NetworkInterface` even though this is inconsistent.
 */
class NetworkInterface extends Model
{
    public StringField $if;
    public BooleanField $enable;
    public StringField $descr;
    public StringField $spoofmac;
    public IntegerField $mtu;
    public IntegerField $mss;
    public StringField $media;
    public StringField $mediaopt;
    public BooleanField $blockpriv;
    public BooleanField $blockbogons;
    public StringField $typev4;
    public StringField $typev6;
    public StringField $ipaddr;
    public IntegerField $subnet;
    public StringField $gateway;

    public function __construct(mixed $id=null, mixed $representation_data=[], Auth $client=null) {
        # Set the configuration path for these objects and allow many to be stored.
        $this->config_path = "interfaces";
        $this->many = true;
        $this->subsystem = "interfaces";
        $this->update_strategy = "replace";

        # Model fields
        $this->if = new StringField(
            required: true,
            unique: true,
            maximum_length: 128,
            help_text: "The physical interface this configuration will be applied to."
        );
        $this->enable = new BooleanField(
            required: false,
            default: false,
            help_text: "Enable or disable this interface."
        );
        $this->descr = new StringField(
            required: true,
            unique: true,
            maximum_length: 128,
            validators: [new FilterNameValidator()],
            help_text: "The descriptive name for this interface."
        );
        $this->spoofmac = new StringField(
            required: false,
            default: "",
            allow_empty: true,
            maximum_length: 17,
            validators: [new MACAddressValidator()],
            help_text: "Assigns (spoofs) the MAC address for this interface instead of using the interface's real MAC."
        );
        $this->mtu = new IntegerField(
            required: false,
            allow_null: true,
            minimum: 1280,
            maximum: 8192,
            help_text: "Sets the MTU for this interface. Assumes default MTU if value is `null`."
        );
        $this->mss = new IntegerField(
            required: false,
            allow_null: true,
            minimum: 576,
            maximum: 65535,
            help_text: "Sets the MSS for this interface. Assumes default MSS if value is `null`."
        );
        $this->media = new StringField(
            required: false,
            default: "",
            allow_empty: true,
            internal_name: "media",
            help_text: "Sets the link speed for this interface. In most situations this can be left as the default."
        );
        $this->mediaopt = new StringField(
            required: false,
            default: "",
            allow_empty: true,
            internal_name: "mediaopt",
            help_text: "Sets the link duplex for this interface. In most situations this can be left as the default."
        );
        $this->blockpriv = new BooleanField(
            required: false,
            default: false,
            help_text: "Enable or disable automatically blocking RFC 1918 private networks on this interface."
        );
        $this->blockbogons = new BooleanField(
            required: false,
            default: false,
            help_text: "Enable or disable automatically blocking bogon networks on this interface."
        );
        $this->typev4 = new StringField(
            choices: ["static", "dhcp", "none"],
            allow_null: true,
            help_text: "Selects the IPv4 address type to assign this interface."
        );
        $this->typev6 = new StringField(
            choices: ["static", "dhcp", "slaac", "6rd", "track6", "6to4", "none"],
            allow_null: true,
            help_text: "Selects the IPv6 address type to assign this interface."
        );
        $this->ipaddr = new StringField(
            required: true,
            internal_name: "ipaddr",
            conditions: ["typev4" => ["static", "dhcp"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false, allow_keywords: ["dhcp"])]
        );
        $this->subnet = new IntegerField(
            required: true,
            minimum: 1,
            maximum: 32,
            internal_name: "subnet",
            conditions: ["typev4" => "static"]
        );
        $this->gateway = new StringField(
            allow_null: true,
            internal_name: "gateway",
            conditions: ["typev4" => "static"],
            validators: [new GatewayValidator(allow_ipv4: true, allow_ipv6: false, interface: $this->id)]
        );

        # Needs to be constructed after fields are defined
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds custom validation to the `if` field.
     * @param string $if The interface value to validate.
     * @returns string The validated $if to set.
     * @throws ValidationError When the `if` value is invalid.
     */
    public function validate_if(string $if) : string {
        # Throw a validation error if the requested real interface does not exist
        if (!does_interface_exist($if)) {
            throw new ValidationError(
                message: "Physical interface `$if` does not exist.",
                response_id: "PHYSICAL_INTERFACE_NOT_FOUND"
            );
        }

        return $if;
    }

    /**
     * Adds custom validation to the `mtu` field.
     * @param int $mtu The MTU value to validate.
     * @returns string The validated $mtu to set.
     * @throws ValidationError When the `if` value is invalid.
     */
    public function validate_mtu(int $mtu) : int {
        # Check if this interface is a VLAN interface
        $vlan_query = InterfaceVLAN::read_all()->query(["vlanif" => $this->if->value]);

        # If this interface is a VLAN interface, ensure the MTU is less than the parent interface
        if ($vlan_query->exists()) {
            $vlan = $vlan_query->model_objects[0];

            # Throw an error if the parent interface's MTU is lower than this interface's MTU
            if (get_interface_mtu($vlan->if->value) < $mtu) {
                throw new ValidationError(
                    message: "MTU cannot be higher than parent interface's MTU",
                    response_id: "INTERFACE_WITH_MTU_HIGHER_THAN_PARENT_MTU"
                );
            }

        }

        return $mtu;
    }

    /**
     * Adds custom validation to the `media` field.
     * @param string $media The media value to validate (e.g. 1000baseT)
     * @returns string The validated $media to set.
     * @throws ValidationError When the `$media` value is invalid.
     */
    public function validate_media(string $media) : string {
        # Variables
        $media_types = array_keys($this->get_supported_media());
        $media_types_str = implode(",", $media_types);

        # Throw an error if the provided media is not supported
        if (!in_array($media, $media_types)) {
            throw new ValidationError(
                message: "Interface does not support media type `$media`, supported options are: [$media_types_str]",
                response_id: "NETWORK_INTERFACE_WITH_UNSUPPORTED_MEDIA_TYPE"
            );
        }

        return $media;
    }

    /**
     * Adds custom validation to the `mediaopt` field.
     * @param string $mediaopt The mediaopt value to validate (e.g. full-duplex)
     * @returns string The validated $mediaopt to set.
     * @throws ValidationError When the `mediaopt` value is invalid.
     */
    public function validate_mediaopt(string $mediaopt) : string {
        # Variables
        $media_options = $this->get_supported_media()[$this->media->value];
        $media_options_str = implode(",", $media_options);

        # Throw an error if the provided media is not supported
        if (!in_array($mediaopt, $media_options)) {
            throw new ValidationError(
                message: "Interface does not support media option `$mediaopt`, supported options are: [$media_options_str]",
                response_id: "NETWORK_INTERFACE_WITH_UNSUPPORTED_MEDIA_OPTION"
            );
        }

        return $mediaopt;
    }

    /**
     * Adds custom validation to the `ipaddr` field.
     * @param string $ipaddr The ipaddr value to validate
     * @returns string The validated $ipaddr to set.
     * @throws ValidationError When the `ipaddr` value is invalid.
     */
    public function validate_ipaddr(string $ipaddr) : string {
        # Force `ipaddr` to be `dhcp` if `typev4` is `dhcp`. Otherwise, retain original value.
        $ipaddr = ($this->typev4->value === "dhcp") ? "dhcp" : $ipaddr;

        # Only allow `dhcp` as a value if the type is also `dhcp`
        if ($ipaddr === "dhcp" and $this->typev4->value !== "dhcp") {
            throw new ValidationError(
                message: "Interface `ipaddr` cannot be `dhcp` unless `typev4` is also `dhcp`",
                response_id: "INTERFACE_IPADDR_IS_DHCP_WITH_NON_DHCP_TYPEV4"
            );
        }

        return $ipaddr;
    }

    /**
     * Overwrites the default get_next_id() method to return the next pfSense interface ID (e.g. 'wan', 'lan', 'opt1').
     * @return string The next available pfSense interface ID
     */
    public function get_next_id() : string {
        # Use `lan` if it doesn't already exist
        if (!$this->read_all()->query(["id" => "lan"])->model_objects) {
            return "lan";
        }

        # Otherwise, loop until we find an available opt interface ID
        foreach (range(1, 65535) as $count) {
            # Use this opt if it doesn't already exist
            if (!$this->read_all()->query(["id" => "opt$count"])->model_objects) {
                return "opt$count";
            }
        }

        throw new ServerError(
            message: "Could not determine the next available interface ID",
            response_id: "COULD_NOT_FIND_NEXT_INTERFACE_ID"
        );
    }

    /**
     * Determines all supported media options for this interface.
     * @return array An array where the key is the speed (e.g. 1000baseT) and the value is an array of duplex options
     */
    public function get_supported_media() : array {
        # Variables
        $media_options = [];
        $if = $this->if->value;

        # Run ifconfig to determine what media and media options are supported
        exec("/sbin/ifconfig -m $if | grep \"media \"", $media_options_output);

        # Loop through each supported media from the ifconfig output
        foreach ($media_options_output as $media_option) {
            # Locate lines beginning with `media `
            preg_match("/media (.*)/", $media_option, $media_line);

            # Check if there is speed AND duplex
            if (preg_match("/(.*) mediaopt (.*)/", $media_line[1], $matches)) {
                $media_options[$matches[1]] = [$matches[2]];
            }
            # Otherwise only include the speed
            else {
                $media_options[$media_line[1]][] = "";
            }
        }

        # Always allow an empty string to assume the default speed and/or duplex
        $media_options[""] = [""];

        return $media_options;
    }

    /**
     * Serializes this interface configuration and places it in the pending interface
     * changes file at /tmp/.interfaces.apply
     */
    public function serialize_changes() {
        # Local variables
        global $g;
        $pending_ifs = [];

        # Check for any existing pending interface changes
        if (file_exists($g['tmp_path']."/.interfaces.apply")) {
            $pending_ifs = unserialize(file_get_contents($g['tmp_path']."/.interfaces.apply"));
        }

        # Set the current validated interface as a pending interface change
        $pending_ifs[$this->id] = [];
        $pending_ifs[$this->id]['ifcfg'] = $this->to_internal();
        file_put_contents($g['tmp_path']."/.interfaces.apply", serialize($pending_ifs));
    }

    public function apply() {
        # Local variables
        global $g;
        $mtu_redo = array();

        # Remove configuration cache if it exists
        unlink_if_exists($g['tmp_path']."/config.cache");

        # Check that there are even pending interface changes to apply
        if (file_exists("{$g['tmp_path']}/.interfaces.apply")) {
            # Load our pending interface file
            $pending_ifs = unserialize(file_get_contents($g['tmp_path']."/.interfaces.apply"));

            # Loop through each pending interface configuration and apply it accordingly
            foreach ($pending_ifs as $if_to_apply => $if_conf) {
                # Check this interface's MTU value
                $if_mtu = get_interface_mtu(get_real_interface($if_to_apply));

                # Only apply changes to interfaces that are enabled
                if ($this->is_config_enabled("interfaces/{$if_to_apply}")) {
                    # Bring down existing interface and configure the new one
                    interface_bring_down($if_to_apply, false, $if_conf);
                    interface_configure($if_to_apply, true);

                    # Call track6 specific configuration steps
                    if ($this->get_config("interfaces/{$if_to_apply}/ipaddrv6") == "track6") {
                        $wancfg = $this->get_config("interfaces/{$if_to_apply}");
                        interface_track6_configure($if_to_apply, $wancfg, true);
                    }
                }
                # If interface was not enabled (or was disabled) remove any existing configurations utilizing interface
                else {
                    # Bring down the existing interface on the system if it exists
                    interface_bring_down($if_to_apply, true, $if_conf);

                    # Restart DHCP services to remove DHCP server(s) on this interface
                    if ($this->is_config_enabled("dhcpd/{$if_to_apply}") or $this->is_config_enabled("dhcpdv6/{$if_to_apply}")) {
                        services_dhcpd_configure();
                    }
                }

                # Interfaces with sub-interfaces (e.g. VLANs) need their MTU reconfigured if changed
                if (interface_has_clones(get_real_interface($if_to_apply))) {
                    # When static MTU is set
                    if ($this->is_config_enabled("interfaces/{$if_to_apply}", "mtu")) {
                        # Interface must be redone if MTU has changed
                        if ($this->get_config("interfaces/{$if_to_apply}/mtu") != $if_mtu) {
                            $mtu_redo[] = get_real_interface($if_to_apply);
                        }
                    }
                    # When default (unspecified) MTU is set
                    else {
                        # Interface must be redone if MTU has changed
                        if (get_interface_default_mtu() != $if_mtu) {
                            $mtu_redo[] = get_real_interface($if_to_apply);
                        }
                    }
                }
            }
        }

        # Reconfigure interface MTU if MTU changes were present
        if (!empty($mtu_redo)) {
            foreach ($mtu_redo as $vlredo) {
                interfaces_vlan_configure_mtu($vlredo);
            }
        }

        # Restart associated/dependent services
        services_snmpd_configure();
        setup_gateways_monitor();
        clear_subsystem_dirty('interfaces');
        filter_configure();
        enable_rrd_graphing();

        # Restart routing services to accommodate interface changes. Clear routing subsystem if it was successful.
        if (is_subsystem_dirty('staticroutes') and system_routing_configure() === 0) {
            clear_subsystem_dirty('staticroutes');
        }

        # Initialize syslog configuration and restart the syslog service
        if ($this->is_config_enabled("syslog") and $if_to_apply == $this->get_config("syslog/sourceip")) {
            system_syslogd_start();
        }

        # After all is said and done, clear the interface subsystem status and remove the pending interface change file
        clear_subsystem_dirty('interfaces');
        @unlink($g['tmp_path']."/.interfaces.apply");
    }

    /**
     * Gathers information about all physical (real) interfaces and returns them in an array.
     * @return array a list of all interfaces along with their attributes
     */
    public static function get_all_avail_interfaces() : array {
        global $ipsec_descrs;
        $base_ifs = get_interface_list();

        # Include wireless clone interfaces
        if (config_get_path('wireless/clone') && is_array(config_get_path('wireless/clone')) && count(config_get_path('wireless/clone'))) {
            foreach (config_get_path('wireless/clone', []) as $clone) {
                $base_ifs[$clone['cloneif']] = $clone;
                $base_ifs[$clone['cloneif']]['iswlclone'] = true;
            }
        }

        # Include VLAN interfaces
        foreach (InterfaceVLAN::read_all()->model_objects as $vlan) {
            $base_ifs[$vlan->vlanif->value] = $vlan->to_internal();
            $base_ifs[$vlan->vlanif]['isvlan'] = true;
        }

        # Include Bridge interfaces
        if (config_get_path('bridges/bridged') && is_array(config_get_path('bridges/bridged')) && count(config_get_path('bridges/bridged'))) {
            foreach (config_get_path('bridges/bridged', []) as $bridge) {
                $base_ifs[$bridge['bridgeif']] = $bridge;
                $base_ifs[$bridge['bridgeif']]['isbridge'] = true;
            }
        }

        # Include GIF interfaces
        if (config_get_path('gifs/gif') && is_array(config_get_path('gifs/gif')) && count(config_get_path('gifs/gif'))) {
            foreach (config_get_path('gifs/gif', []) as $gif) {
                $base_ifs[$gif['gifif']] = $gif;
                $base_ifs[$gif['gifif']]['isgif'] = true;
            }
        }

        # Include GRE interfaces
        if (config_get_path('gres/gre') && is_array(config_get_path('gres/gre')) && count(config_get_path('gres/gre'))) {
            foreach (config_get_path('gres/gre', []) as $gre) {
                $base_ifs[$gre['greif']] = $gre;
                $base_ifs[$gre['greif']]['isgre'] = true;
            }
        }

        # Include LAGG interfaces
        if (config_get_path('laggs/lagg') && is_array(config_get_path('laggs/lagg')) && count(config_get_path('laggs/lagg'))) {
            foreach (config_get_path('laggs/lagg', []) as $lagg) {
                $base_ifs[$lagg['laggif']] = $lagg;
                $base_ifs[$lagg['laggif']]['islagg'] = true;
                # LAGG members cannot be assigned
                $lagifs = explode(',', $lagg['members']);
                foreach ($lagifs as $lagif) {
                    if (isset($base_ifs[$lagif])) {
                        unset($base_ifs[$lagif]);
                    }
                }
            }
        }

        # Include QinQ interfaces
        if (config_get_path('qinqs/qinqentry') && is_array(config_get_path('qinqs/qinqentry')) && count(config_get_path('qinqs/qinqentry'))) {
            foreach (config_get_path('qinqs/qinqentry', []) as $qinq) {
                $base_ifs["{$qinq['vlanif']}"]['descr'] = "VLAN {$qinq['tag']} on {$qinq['if']}";
                $base_ifs["{$qinq['vlanif']}"]['isqinq'] = true;
                # QinQ members
                $qinqifs = explode(' ', $qinq['members']);
                foreach ($qinqifs as $qinqif) {
                    $base_ifs["{$qinq['vlanif']}.{$qinqif}"]['descr'] = "QinQ {$qinqif} on VLAN {$qinq['tag']} on {$qinq['if']}";
                    $base_ifs["{$qinq['vlanif']}.{$qinqif}"]['isqinq'] = true;
                }
            }
        }

        # Include PPP interfaces
        if (config_get_path('ppps/ppp') && is_array(config_get_path('ppps/ppp')) && count(config_get_path('ppps/ppp'))) {
            foreach (config_get_path('ppps/ppp', []) as $pppid => $ppp) {
                $portname = $ppp['if'];
                $base_ifs[$portname] = $ppp;
                $base_ifs[$portname]['isppp'] = true;
                $ports_base = basename($ppp['ports']);
                if (isset($ppp['descr'])) {
                    $base_ifs[$portname]['descr'] = strtoupper($ppp['if']). "({$ports_base}) - {$ppp['descr']}";
                } else if (isset($ppp['username'])) {
                    $base_ifs[$portname]['descr'] = strtoupper($ppp['if']). "({$ports_base}) - {$ppp['username']}";
                } else {
                    $base_ifs[$portname]['descr'] = strtoupper($ppp['if']). "({$ports_base})";
                }
            }
        }

        # Include OpenVPN interfaces
        $ovpn_descrs = array();
        if (config_get_path('openvpn') && is_array(config_get_path('openvpn'))) {
            if (config_get_path('openvpn/openvpn-server') && is_array(config_get_path('openvpn/openvpn-server'))) {
                foreach (config_get_path('openvpn/openvpn-server', []) as $s) {
                    $portname = "ovpns{$s['vpnid']}";
                    $base_ifs[$portname] = $s;
                    $ovpn_descrs[$s['vpnid']] = $s['description'];
                }
            }
            if (config_get_path('openvpn/openvpn-client') && is_array(config_get_path('openvpn/openvpn-client'))) {
                foreach (config_get_path('openvpn/openvpn-client', []) as $c) {
                    $portname = "ovpnc{$c['vpnid']}";
                    $base_ifs[$portname] = $c;
                    $ovpn_descrs[$c['vpnid']] = $c['description'];
                }
            }
        }

        # Include IPsec interfaces
        $ipsec_descrs = interface_ipsec_vti_list_all();
        foreach ($ipsec_descrs as $ifname => $ifdescr) {
            $base_ifs[$ifname] = array('descr' => $ifdescr);
        }

        # Loop through our array and check if interface is in use
        foreach (self::read_all()->model_objects as $interface) {
                $base_ifs[$interface->if->value]["in_use"] = true;
        }

        return $base_ifs;
    }
}
