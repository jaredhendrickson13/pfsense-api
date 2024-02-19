<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\Base64Field;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\InterfaceField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ConflictError;
use RESTAPI\Validators\HostnameValidator;
use RESTAPI\Validators\IPAddressValidator;
use RESTAPI\Validators\SubnetValidator;

class OpenVPNServer extends Model
{
    public IntegerField $vpnid;
    public StringField $description;
    public BooleanField $disable;
    public StringField $mode;
    public ForeignModelField $authmode;
    public StringField $dev_mode;
    public StringField $protocol;
    public InterfaceField $interface;
    public PortField $local_port;
    public Base64Field $tls;
    public StringField $tls_type;
    public StringField $tlsauth_keydir;
    public ForeignModelField $caref;
    public ForeignModelField $certref;
    public IntegerField $cert_depth;
    public StringField $dh_length;
    public StringField $ecdh_curve;
    public StringField $data_ciphers;
    public StringField $data_ciphers_fallback;
    public StringField $digest;
    public BooleanField $remote_cert_tls;
    public StringField $tunnel_network;
    public StringField $tunnel_networkv6;
    public BooleanField $serverbridge_dhcp;
    public InterfaceField $serverbridge_interface;
    public BooleanField $serverbridge_routegateway;
    public StringField $serverbridge_dhcp_start;
    public StringField $serverbridge_dhcp_end;
    public BooleanField $gwredir;
    public BooleanField $gwredir6;
    public StringField $local_network;
    public StringField $local_networkv6;
    public StringField $remote_network;
    public StringField $remote_networkv6;
    public IntegerField $maxclients;
    public StringField $allow_compression;
    public BooleanField $passtos;
    public BooleanField $client2client;
    public BooleanField $duplicate_cn;
    public IntegerField $connlimit;
    public BooleanField $dynamic_ip;
    public StringField $topology;
    public IntegerField $inactive_seconds;
    public StringField $ping_method;
    public IntegerField $keepalive_interval;
    public IntegerField $keepalive_timeout;
    public IntegerField $ping_seconds;
    public BooleanField $ping_push;
    public StringField $ping_action;
    public IntegerField $ping_action_seconds;
    public BooleanField $ping_action_push;
    public StringField $dns_domain;
    public StringField $dns_server1;
    public StringField $dns_server2;
    public StringField $dns_server3;
    public StringField $dns_server4;
    public BooleanField $push_blockoutsidedns;
    public BooleanField $push_register_dns;
    public StringField $ntp_server1;
    public StringField $ntp_server2;
    public BooleanField $netbios_enable;
    public IntegerField $netbios_ntype;
    public StringField $netbios_scope;
    public StringField $wins_server1;
    public StringField $wins_server2;
    public StringField $custom_options;
    public BooleanField $username_as_common_name;
    public IntegerField $sndrcvbuf;
    public StringField $create_gw;
    public IntegerField $verbosity_level;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->many = true;
        $this->config_path = "openvpn/openvpn-server";
        $this->always_apply = true;

        # Set model fields
        $this->vpnid = new IntegerField(
            unique: true,
            default_callable: "get_next_vpn_id",
            editable: false,
            read_only: true,
            help_text: "The unique ID for this OpenVPN server. This value is assigned by the ".
                "system and cannot be changed."
        );
        $this->description = new StringField(
            default: "",
            allow_empty: true,
            help_text: "The description for this OpenVPN server."
        );
        $this->disable = new BooleanField(
            default: false,
            help_text: "Disables this OpenVPN server."
        );
        $this->mode = new StringField(
            required: true,
            choices: ["p2p_tls", "server_tls", "server_user", "server_tls_user"],
            help_text: "The OpenVPN server mode."
        );
        $this->authmode = new ForeignModelField(
            model_name: "AuthServer",
            model_field: "name",
            default: "Local Database",
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            help_text: "The name of the authentication server to use as the authentication ".
                "backend for this OpenVPN server"
        );
        $this->dev_mode = new StringField(
            required: true,
            choices: ["tun", "tap"],
            help_text: "The carrier mode for this OpenVPN server. `tun` mode carries IPv4 and IPv6 (layer 3) and ".
                "is the most common and compatible mode across all platforms. `tap` mode is capable of carrying ".
                "802.3 (layer 2)."
        );
        $this->protocol = new StringField(
            required: true,
            choices: ["UDP4", "UDP6", "UDP", "TCP4", "TCP6", "TCP"],
            help_text: "The protocol used by this OpenVPN server."
        );
        $this->interface = new InterfaceField(
            required: true,
            allow_carp_interface: true,
            conditions: ["!protocol" => ["UDP", "TCP"]],
            help_text: "The interface or Virtual IP address where OpenVPN will receive client connections."
        );
        $this->local_port = new PortField(
            unique: true,
            default: "1194",
            allow_alias: false,
            allow_range: false,
            help_text: "The port used by OpenVPN to receive client connections."
        );
        $this->tls = new Base64Field(
            default: null,
            allow_null: true,
            help_text: "The TLS key this OpenVPN server will use to sign control channel packets with an HMAC ".
                "signature for authentication when establishing the tunnel."
        );
        $this->tls_type = new StringField(
            required: true,
            choices: ["auth", "crypt"],
            conditions: ["!tls" => null],
            help_text: "The TLS key usage type. In `auth` mode, the TLS key is used only as HMAC authentication for ".
                "the control channel, protecting the peers from unauthorized connections. The `crypt` mode encrypts ".
                "the control channel communication in addition to providing authentication, providing more privacy ".
                "and traffic control channel obfuscation."
        );
        $this->tlsauth_keydir = new StringField(
            default: "default",
            choices: ["default", "0", "1", "2"],
            conditions: ["!tls" => null],
            help_text: "The TLS key direction. This must be set to complementary values on the client and server. ".
                "For example, if the server is set to 0, the client must be set to 1. Both may be set to omit the ".
                "direction, in which case the TLS Key will be used bidirectionally."
        );
        $this->caref = new ForeignModelField(
            model_name: "CertificateAuthority",
            model_field: "refid",
            required: true,
            help_text: "The `refid` of the CA object to assume as the peer CA."
        );
        $this->certref = new ForeignModelField(
            model_name: "Certificate",
            model_field: "refid",
            required: true,
            help_text: "The `refid` of the certificate object to assume as the OpenVPN server certificate."
        );
        $this->cert_depth = new IntegerField(
            default: 1,
            choices: [1, 2, 3, 4, 5],
            allow_null: true,
            help_text: "The depth of the certificate chain to check when a certificate based client signs in. ".
                "Certificates below this depth are not accepted. This is useful for denying certificates made with ".
                "intermediate CAs generated from the same CA as the server. Set to null to use system default."
        );
        $this->dh_length = new StringField(
            required: true,
            choices_callable: "get_dh_length_choices",
            help_text: "The Diffie-Hellman (DH) parameter set used for key exchange."
        );
        $this->ecdh_curve = new StringField(
            required: true,
            choices_callable: "get_ecdh_curve_choices",
            help_text: "The Elliptic Curve to use for key exchange. The curve from the server certificate is used by ".
                "default when the server uses an ECDSA certificate. Otherwise, secp384r1 is used as a fallback."
        );
        $this->data_ciphers = new StringField(
            required: true,
            choices_callable: "get_available_cipher_choices",
            many: true,
            many_minimum: 1,
            help_text: "The encryption algorithms/ciphers allowed by this OpenVPN server."
        );
        $this->data_ciphers_fallback = new StringField(
            required: true,
            choices_callable: "get_available_cipher_choices",
            help_text: "The fallback encryption algorithm/cipher used for data channel packets when communicating ".
                "with clients that do not support data encryption algorithm negotiation (e.g. Shared Key)."
        );
        $this->digest = new StringField(
            required: true,
            choices_callable: "openvpn_get_digestlist",
            help_text: "The algorithm used to authenticate data channel packets, and control channel packets ".
                "if a TLS Key is present."
        );
        $this->remote_cert_tls = new BooleanField(
            default: true,
            help_text: "Enable or disable requiring hosts to have a client certificate to connect."
        );
        $this->tunnel_network = new StringField(
            default: "",
            allow_empty: true,
            validators: [new SubnetValidator(allow_ipv4: true, allow_ipv6: false, allow_alias: false)],
            help_text: "The IPv4 virtual network used for private communications between this server and client hosts."
        );
        $this->tunnel_networkv6 = new StringField(
            default: "",
            allow_empty: true,
            validators: [new SubnetValidator(allow_ipv4: false, allow_ipv6: true, allow_alias: false)],
            help_text: "The IPv6 virtual network used for private communications between this server and client hosts."
        );
        $this->serverbridge_dhcp = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["dev_mode" => "tun"],
            help_text: "Enables or disables clients on the bridge to obtain DHCP."
        );
        $this->serverbridge_interface = new InterfaceField(
            required: true,
            conditions: ["serverbridge_dhcp" => true],
            help_text: "The interface to which this TAP instance will be bridged. This is not done automatically. ".
                "This interface must be assigned and the bridge created separately. This setting controls which ".
                "existing IP address and subnet mask are used by OpenVPN for the bridge."
        );
        $this->serverbridge_routegateway = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["serverbridge_dhcp" => true],
            help_text: "Enables or disables pushing the bridge interface's IPv4 address to connecting clients as ".
                "a route gateway."
        );
        $this->serverbridge_dhcp_start = new StringField(
            required: true,
            conditions: ["serverbridge_dhcp" => true],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The bridge DHCP range's start address."
        );
        $this->serverbridge_dhcp_end = new StringField(
            required: true,
            conditions: ["serverbridge_dhcp" => true],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The bridge DHCP range's end address."
        );
        $this->gwredir = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            help_text: "Enable forcing all client-generated IPv4 traffic through the tunnel."
        );
        $this->gwredir6 = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            help_text: "Enable forcing all client-generated IPv6 traffic through the tunnel."
        );
        $this->local_network = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            conditions: ["gwredir" => false],
            validators: [new SubnetValidator(allow_ipv4: true, allow_ipv6: false, allow_alias: true)],
            help_text: "The IPv4 networks that will be accessible from the remote endpoint. Expressed as a ".
                "list of one or more CIDR ranges or host/network type aliases. This may be left blank if not adding ".
                "a route to the local network through this tunnel on the remote machine. This is generally set to ".
                "the LAN network."
        );
        $this->local_networkv6 = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            conditions: ["gwredir6" => false],
            validators: [new SubnetValidator(allow_ipv4: false, allow_ipv6: true, allow_alias: true)],
            help_text: "The IPv6 networks that will be accessible from the remote endpoint. Expressed as a ".
                "list of one or more CIDR ranges or host/network type aliases. This may be left blank if not adding ".
                "a route to the local network through this tunnel on the remote machine. This is generally set to ".
                "the LAN network."
        );
        $this->remote_network = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            validators: [new SubnetValidator(allow_ipv4: true, allow_ipv6: false, allow_alias: true)],
            help_text: "IPv4 networks that will be routed through the tunnel, so that a site-to-site VPN can be ".
                "established without manually changing the routing tables. Expressed as a list of ".
                "one or more CIDR ranges or host/network type aliases. If this is a site-to-site VPN, enter the ".
                "remote LAN/s here. May be left empty for non site-to-site VPN."
        );
        $this->remote_networkv6 = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            validators: [new SubnetValidator(allow_ipv4: false, allow_ipv6: true, allow_alias: true)],
            help_text: "IPv6 networks that will be routed through the tunnel, so that a site-to-site VPN can be ".
                "established without manually changing the routing tables. Expressed as a list of ".
                "one or more CIDR ranges or host/network type aliases. If this is a site-to-site VPN, enter the ".
                "remote LAN/s here. May be left empty for non site-to-site VPN."
        );
        $this->maxclients = new IntegerField(
            default: null,
            allow_null: true,
            help_text: "the maximum number of clients allowed to concurrently connect to this server."
        );
        $this->allow_compression = new StringField(
            default: "no",
            choices: ["no", "yes", "asym"],
            help_text: "The compression mode allowed by this OpenVPN server. Compression can potentially increase ".
                "throughput but may allow an attacker to extract secrets if they can control compressed plaintext ".
                "traversing the VPN (e.g. HTTP)"
        );
        $this->passtos = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            help_text: "Enables or disables setting the TOS IP header value of tunnel packets to match the ".
                "encapsulated packet value."
        );
        $this->client2client = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            help_text: "Enables or disables allowing communication between clients connected to this server."
        );
        $this->duplicate_cn = new BooleanField(
            default: false,
            help_text: "Enables or disable allowing the same user to connect multiple times."
        );
        $this->connlimit = new IntegerField(
            default: null,
            allow_null: true,
            conditions: ["duplicate_cn" => true],
            help_text: "The number of concurrent connections a single user can have."
        );
        $this->dynamic_ip = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            help_text: "Enables or disables allowing connected clients to retain their connections if ".
                "their IP address changes."
        );
        $this->topology = new StringField(
            default: "subnet",
            choices: ["subnet", "net30"],
            conditions: ["dev_mode" => "tun"],
            help_text: "The method used to supply a virtual adapter IP address to clients when using TUN mode on IPv4."
        );
        $this->inactive_seconds = new IntegerField(
            default: 300,
            help_text: "The amount of time (in seconds) until a client connection is closed for inactivity."
        );
        $this->ping_method = new StringField(
            default: "keepalive",
            choices: ["keepalive", "ping"],
            help_text: "The method used to define ping configuration."
        );
        $this->keepalive_interval = new IntegerField(
            default: 10,
            conditions: ["ping_method" => "keepalive"],
            help_text: "The keepalive interval parameter."
        );
        $this->keepalive_timeout = new IntegerField(
            default: 60,
            conditions: ["ping_method" => "keepalive"],
            help_text: "The keepalive timeout parameter."
        );
        $this->ping_seconds = new IntegerField(
            default: 10,
            conditions: ["ping_method" => "ping"],
            help_text: "The number of seconds to accept no packets before sending a ping to the ".
                "remote peer over the TCP/UDP control channel."
        );
        $this->ping_push = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["ping_method" => "ping"],
            help_text: "Enables or disables push ping to the VPN client."
        );
        $this->ping_action = new StringField(
            default: "ping_restart",
            choices: ["ping_restart", "ping_exit"],
            conditions: ["ping_method" => "ping"],
            help_text: "The action to take after a ping to the remote peer times-out."
        );
        $this->ping_action_seconds = new IntegerField(
            default: 60,
            conditions: ["ping_method" => "ping"],
            help_text: "The number of seconds that must elapse before the ping is considered a timeout and the ".
                "configured `ping_action` is performed."
        );
        $this->ping_action_push = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["ping_method" => "ping"],
            help_text: "Enables or disables pushing the ping action to the VPN client."
        );
        $this->dns_domain = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new HostnameValidator(allow_hostname: true, allow_domain: true, allow_fqdn: true)],
            help_text: "The default domain to provide to clients."
        );
        $this->dns_server1 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The primary DNS server to provide to clients."
        );
        $this->dns_server2 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The secondary DNS server to provide to clients."
        );
        $this->dns_server3 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The tertiary DNS server to provide to clients."
        );
        $this->dns_server4 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The quaternary DNS server to provide to clients."
        );
        $this->push_blockoutsidedns = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            help_text: "Enables or disables blocking Windows 10 clients' access to DNS servers except across OpenVPN ".
                "while connected, forcing clients to use only VPN DNS servers."
        );
        $this->push_register_dns = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            help_text: "Enables or disables running `net stop dnscache`, `net start dnscache`, `ipconfig /flushdns` ".
                "and `ipconfig /registerdns` on connection initiation for Windows clients."
        );
        $this->ntp_server1 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The primary NTP server to provide to clients."
        );
        $this->ntp_server2 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The secondary NTP server to provide to clients."
        );
        $this->netbios_enable = new BooleanField(
            default: false,
            indicates_true: "yes",
            indicates_false: "",
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            help_text: "Enables or disables NetBIOS over TCP/IP."
        );
        $this->netbios_ntype = new IntegerField(
            default: 0,
            choices: [0, 1, 2, 4, 8],
            conditions: ["netbios_enable" => true],
            help_text: "The NetBIOS node type."
        );
        $this->netbios_scope = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["netbios_enable" => true],
            help_text: "The NetBIOS Scope ID. This provides an extended naming service for NetBIOS over TCP/IP. The ".
                "NetBIOS scope ID isolates NetBIOS traffic on a single network to only those nodes with the same ".
                "NetBIOS scope ID."
        );
        $this->wins_server1 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The primary WINS server to provide to clients."
        );
        $this->wins_server2 = new StringField(
            default: "",
            allow_empty: true,
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true)],
            help_text: "The secondary WINS server to provide to clients."
        );
        $this->custom_options = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            delimiter: ";",
            help_text: "Additional options to add to the OpenVPN server configuration."
        );
        $this->username_as_common_name = new BooleanField(
            default: false,
            indicates_true: "enabled",
            indicates_false: "disabled",
            conditions: ["mode" => ["server_user", "server_tls_user"]],
            help_text: "Enables or disable the username of the client being used in place of the certificate common ".
                "name for purposes such as determining Client Specific Overrides."
        );
        $this->sndrcvbuf = new IntegerField(
            default: null,
            choices: [65536, 131072, 262144, 524288, 1048576, 2097152],
            allow_null: true,
            help_text: "The send and receive buffer size for OpenVPN. Set to null to use the system default."
        );
        $this->create_gw = new StringField(
            default: "both",
            choices: ["both", "v4only", "v6only"],
            help_text: "The gateway type(s) that will be created when a virtual interface is assigned ".
                "to this OpenVPN server"
        );
        $this->verbosity_level = new IntegerField(
            default: 1,
            choices: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
            help_text: "The OpenVPN logging verbosity level."
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Adds extra validation to the `disable` field.
     * @param bool $disable The incoming value to validate.
     * @return bool The validated value to be assigned.
     * @throws ConflictError When $disable is true, but this OpenVPNServer is in use by an existing interface.
     */
    public function validate_disable(bool $disable): bool {
        # Do not allow this OpenVPN server to be disabled if it's in use by an interface
        if ($disable and openvpn_inuse($this->vpnid->value, 'server')) {
            throw new ConflictError(
                message: "Field `disable` cannot be set to `true` while this OpenVPNServer is in use.",
                response_id: "OPENVPN_SERVER_CANNOT_BE_DISABLED_WHILE_IN_USE"
            );
        }
        
        return $disable;
    }

    /**
     * Obtains the next available VPN ID.
     * @return integer The next available ID number.
     */
    protected function get_next_vpn_id(): int {
        return openvpn_vpnid_next();
    }

    /**
     * Obtains the `dh_length` field choices.
     * @return array An array of current `dh_length` choices for this system
     */
    protected function get_dh_length_choices(): array {
        # Variables
        global $openvpn_dh_lengths;
        $choices = [];

        # We need the array keys to be strings, loop through the options and format a new array
        foreach ($openvpn_dh_lengths as $choice => $choice_verbose) {
            $choices[strval($choice)] = $choice_verbose;
        }

        return $choices;
    }

    /**
     * Obtains the current `ecdh_curve` field choices.
     * @return array An array of the current `ecdh_curve` field choices.
     */
    protected function get_ecdh_curve_choices(): array {
        return openvpn_get_curvelist();
    }

    /**
     * Obtains the available cipher options for this system.
     * @return array An array of available cipher options for this system.
     */
    protected function get_available_cipher_choices(): array {
        return openvpn_get_cipherlist();
    }

    /**
     * Obtains the available `digest` options for this system.
     * @return array An array of available digest algorithms for this system.
     */
    protected function get_avialable_digest_choices(): array {
        return openvpn_get_digestlist();
    }
    
    /**
     * Overrides the parent _delete() method to ensure this OpenVPNServer is not in use before deletion.
     */
    public function _delete(): void
    {
        # Do not allow this OpenVPN server to be deleted if it's in use by an interface
        if (openvpn_inuse($this->vpnid->value, 'server')) {
            throw new ConflictError(
                message: "OpenVPNServer cannot be deleted while it is in use.",
                response_id: "OPENVPN_SERVER_CANNOT_BE_DELETED_WHILE_IN_USE"
            );
        }
        parent::_delete();
    }
}
