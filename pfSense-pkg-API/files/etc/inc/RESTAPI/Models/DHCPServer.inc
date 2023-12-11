<?php

namespace RESTAPI\Models;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\IntegerField;
use RESTAPI\Fields\StringField;
use RESTAPI\Responses\ValidationError;
use RESTAPI\Validators\IPAddressValidator;
use RESTAPI\Validators\MACAddressValidator;

/*
 * Defines a Model that configures the DHCP server for a given interface.
 */
class DHCPServer extends Model
{
    public BooleanField $enable;
    public StringField $range_from;
    public StringField $range_to;
    public StringField $domain;
    public StringField $failover_peerip;
    public StringField $mac_allow;
    public StringField $mac_deny;
    public StringField $domainsearchlist;
    public IntegerField $defaultleasetime;
    public IntegerField $maxleasetime;
    public StringField $gateway;
    public StringField $dnsserver;
    public StringField $winsserver;
    public StringField $ntpserver;
    public BooleanField $staticarp;
    public BooleanField $ignorebootp;
    public BooleanField $ignoreclientuids;
    public BooleanField $nonak;
    public BooleanField $disablepingcheck;
    public BooleanField $dhcpleaseinlocaltime;
    public BooleanField $statsgraph;
    public StringField $denyunknown;

    /**
     * Constructs the Model object.
     */
    public function __construct(mixed $id = null, mixed $data = [], Auth $client = null)
    {
        # Define Model attributes
        $this->config_path = "dhcpd";
        $this->many = true;
        $this->always_apply = true;
        $this->update_strategy = "merge";
        $this->verbose_name = "DHCP Server";

        # Define Model Fields
        $this->enable = new BooleanField(
            default: false,
            help_text: "Enable the DHCP server for this interface."
        );
        $this->range_from = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 15,
            internal_name: "from",
            internal_namespace: "range",
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The starting IP address for the primary DHCP pool. This address must be less than or equal ".
            "to the `range_to` field."
        );
        $this->range_to = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 15,
            internal_name: "to",
            internal_namespace: "range",
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The ending IP address for the primary DHCP pool. This address must be greater than or equal ".
            "to the `range_to` field."
        );
        $this->domain = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 255,
            help_text: "The domain to be assigned via DHCP."
        );
        $this->failover_peerip = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 255,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_fqdn: true)],
            help_text: "The interface IP address of the other firewall (failover peer) in this subnet. Leave ".
            "empty to disable failover peering."
        );
        $this->mac_allow = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            maximum_length: 17,
            validators: [new MACAddressValidator()],
            help_text: "MAC addresses this DHCP server is allowed to provide leases for."
        );
        $this->mac_deny = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            maximum_length: 17,
            validators: [new MACAddressValidator()],
            help_text: "MAC addresses this DHCP server is not allowed to provide leases for."
        );
        $this->domainsearchlist = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            maximum_length: 255,
            delimiter: ";",
            validators: [new IPAddressValidator(allow_ipv4: false, allow_ipv6: false, allow_fqdn: true)],
            help_text: "The domain search list to provide via DHCP."
        );
        $this->defaultleasetime = new IntegerField(
            default: 7200,
            allow_null: true,
            minimum: 60,
            help_text: "The default DHCP lease validity period (in seconds). This is used for clients that do not ask ".
            "for a specific expiration time."
        );
        $this->maxleasetime = new IntegerField(
            default: 86400,
            allow_null: true,
            minimum: 60,
            help_text: "The maximum DHCP lease validity period (in seconds) a client can request."
        );
        $this->gateway = new StringField(
            default: "",
            allow_empty: true,
            maximum_length: 15,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false, allow_keywords: ["none"])],
            help_text: "The gateway IPv4 address to provide via DHCP. This is only necessary if you are not using ".
            "the interface's IP as the gateway. Specify `none` for no gateway assignment."
        );
        $this->dnsserver = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            many_maximum: 4,
            maximum_length: 15,
            delimiter: null,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The DNS servers to provide via DHCP. Leave empty to default to system nameservers."
        );
        $this->winsserver = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            many_maximum: 2,
            maximum_length: 15,
            delimiter: null,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The WINS servers to provide via DHCP."
        );
        $this->ntpserver = new StringField(
            default: [],
            allow_empty: true,
            many: true,
            many_maximum: 3,
            maximum_length: 256,
            delimiter: null,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false, allow_fqdn: true)],
            help_text: "The NTP servers to provide via DHCP."
        );
        $this->staticarp = new BooleanField(
            default: false,
            help_text: "Assign static ARP entries for DHCP leases provided by this server."
        );
        $this->ignorebootp = new BooleanField(
            default: false,
            help_text: "Force this DHCP server to ignore BOOTP queries."
        );
        $this->ignoreclientuids = new BooleanField(
            default: false,
            help_text: "Prevent recording a unique identifier (UID) in client lease data if present in the client ".
            "DHCP request. This option may be useful when a client can dual boot using different client ".
            "identifiers but the same hardware (MAC) address. Note that the resulting server behavior violates ".
            "the official DHCP specification."
        );
        $this->nonak = new BooleanField(
            default: false,
            help_text: "Ignore denied clients rather than reject. This option is not compatible with failover and ".
            "cannot be enabled when a Failover Peer IP address is configured."
        );
        $this->disablepingcheck = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Prevent the DHCP server from sending a ping to the address being assigned, where if no ".
            "response has been heard, it assigns the address."
        );
        $this->dhcpleaseinlocaltime = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Display the DHCP lease times in local time instead of UTC."
        );
        $this->statsgraph = new BooleanField(
            default: false,
            indicates_true: "yes",
            help_text: "Enable adding DHCP lease statistics to the pfSense Monitoring graphs."
        );
        $this->denyunknown = new StringField(
            default: null,
            choices: ["enabled", "class"],
            allow_empty: true,
            allow_null: true,
            help_text: "Define how to handle unknown clients requesting DHCP leases. When set to `null`, any DHCP ".
            "client will get an IP address within this scope/range on this interface. If set to `enabled`, ".
            "any DHCP client with a MAC address listed in a static mapping on any scope(s)/interface(s) will ".
            "get an IP address. If set to `class`, only MAC addresses listed in static mappings on this interface ".
            "will get an IP address within this scope/range."
        );

        parent::__construct(id: $id, data: $data, client: $client);
    }

    /**
     * Provides extra validation for the `enable` field.
     * @param bool $enable The incoming `enable` field value to validate.
     * @return bool The validated `enable` field value to be assigned.
     * @throws ValidationError When `enable` is `true` but interface associated with `id` does not have a static IP.
     * @throws ValidationError When `enable` is `true`, but there is a DHCP relay running on this interface.
     */
    public function validate_enable(bool $enable) : bool {
        # Get the interface associated with this DHCP server
        $interface = NetworkInterface::query(["id" => $this->id])->first();

        # Do not allow the DHCP server to be enabled if the interface does not have a static IP address assigned
        if ($enable and $interface->typev4->value !== "static") {
            throw new ValidationError(
                message: "DHCP server cannot be enabled because interface `{$this->id}` does not have a static IPv4 ".
                    "address assigned.",
                response_id: "DHCP_SERVER_CANNOT_ENABLE_WITHOUT_STATIC_IPV4"
            );
        }

        # Do not allow the DHCP server to be enabled if this interface is already running a DHCP relay
        # TODO: Replace this with a Model query when the DHCPRelay Model is developed
        foreach ($this->get_config("dhcrelay", []) as $dhcp_relay) {
            if ($this->id === $dhcp_relay["interface"]) {
                throw new ValidationError(
                    message: "DHCP server cannot be enabled because a DHCP relay is already running.",
                    response_id: "DHCP_SERVER_CANNOT_BE_ENABLED_WITH_DHCP_RELAY"
                );
            }
        }

        return $enable;
    }

    /**
     * Provides extra validation for the `range_from` field.
     * @param string $range_from The incoming `range_from` field value to validate.
     * @return string The validated `range_from` field value to be assigned.
     * @throws ValidationError When `range_from` is the interface's network address.
     * @throws ValidationError When `range_from` is not in the interface's subnet.
     * @throws ValidationError When `range_from` is greater than the `range_to` IP.
     */
    public function validate_range_from(string $range_from) : string {
        # Get the parent interface for this DHCP server
        $interface = NetworkInterface::query(["id" => $this->id])->first();

        # Do not allow this IP to be the network address
        if ($interface->get_network_ipv4() == $range_from) {
            throw new ValidationError(
                message: "DHCP server `range_from` cannot be the interface subnet's network address.",
                response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_NETWORK_ADDRESS"
            );
        }

        # Do not allow the `range_from` to be greater `range_to`
        if (ip_greater_than($range_from, $this->range_to->value)) {
            throw new ValidationError(
                message: "DHCP server `range_from` cannot be greater than `range_to`.",
                response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_GREATER_THAN_RANGE_TO"
            );
        }

        # Do not allow the `range_from` IP to be outside the interface's configured subnet
        if (!is_inrange_v4($range_from, $interface->get_network_ipv4(), $interface->get_broadcast_ipv4())) {
            throw new ValidationError(
                message: "DHCP server `range_from` must lie within the interface's subnet.",
                response_id: "DHCP_SERVER_RANGE_FROM_OUTSIDE_OF_SUBNET"
            );
        }

        return $range_from;
    }

    /**
     * Provides extra validation for the `range_from` field.
     * @param string $range_to The incoming `range_from` field value to validate.
     * @return string The validated `range_from` field value to be assigned.
     * @throws ValidationError When `range_from` is the interface's network address.
     * @throws ValidationError When `range_from` is not in the interface's subnet.
     * @throws ValidationError When `range_from` is greater than the `range_to` IP.
     */
    public function validate_range_to(string $range_to) : string {
        # Get the parent interface for this DHCP server
        $interface = NetworkInterface::query(["id" => $this->id])->first();

        # Do not allow this IP to be the network address
        if ($interface->get_broadcast_ipv4() === $range_to) {
            throw new ValidationError(
                message: "DHCP server `range_to` cannot be the interface subnet's broadcast address.",
                response_id: "DHCP_SERVER_RANGE_FROM_CANNOT_BE_BROADCAST_ADDRESS"
            );
        }

        # Do not allow the `range_to` IP to be outside the interface's configured subnet
        if (!is_inrange_v4($range_to, $interface->get_network_ipv4(), $interface->get_broadcast_ipv4())) {
            throw new ValidationError(
                message: "DHCP server `range_to` must lie within the interface's subnet.",
                response_id: "DHCP_SERVER_RANGE_TO_OUTSIDE_OF_SUBNET"
            );
        }

        # Do not allow the primary DHCP pool to overlap with an existing virtual IP
        foreach (VirtualIP::read_all()->model_objects as $vip) {
            if (is_inrange_v4($vip->subnet->value, $this->range_from->value, $range_to)) {
                throw new ValidationError(
                    message: "DHCP server primary pool overlaps virtual IP with ID `$vip->id`.",
                    response_id: "DHCP_SERVER_POOL_OVERLAPS_VIP"
                );
            }
        }

        # Do not allow the primary DHCP pool to overlap with any existing DHCP static mappings
        # TODO: Use a Model query for this loop when the DHCPServerStaticMapping Model is developed
        foreach ($this->get_config("dhcpd/$this->id/staticmap", []) as $id => $static_mapping) {
            if (is_inrange_v4($static_mapping['ipaddr'], $this->range_from->value, $range_to)) {
                throw new ValidationError(
                    message: "DHCP server primary pool overlaps with DHCP server static mapping with ID `$id`.",
                    response_id: "DHCP_SERVER_POOL_OVERLAPS_STATIC_MAPPING"
                );
            }
        }

        return $range_to;
    }

    /**
     * Provides extra validation for the `maxleasetime` field.
     * @param int $maxleasetime The incoming `maxleasetime` field value to validate.
     * @return int The validated `maxleasetime` field value to be assigned.
     * @throws ValidationError When `maxleasetime` is less than the `defaultleasetime`
     */
    public function validate_maxleasetime(int $maxleasetime) : int {
        if ($maxleasetime < $this->defaultleasetime->value) {
            throw new ValidationError(
                message: "DHCP server `maxleasetime` cannot be less than the `defaultleasetime`",
                response_id: "DHCP_SERVER_MAX_LEASE_TIME_LESS_THAN_DEFAULT"
            );
        }

        return $maxleasetime;
    }

    /**
     * Provides extra validation for the `gateway` field.
     * @param string $gateway The incoming `gateway` field value to validate.
     * @return string The validated `gateway` field value to be assigned.
     * @throws ValidationError When `gateway` is not an IP within the interface's subnet
     */
    public function validate_gateway(string $gateway) : string {
        # Get the parent interface for this DHCP server
        $interface = NetworkInterface::query(["id" => $this->id])->first();

        # Do not allow non-empty gateway values that are not within the interface's subnet
        if ($gateway and !ip_in_subnet($gateway, $interface->get_current_ipv4()."/".$interface->get_current_subnetv4())) {
            throw new ValidationError(
                message: "DHCP server `gateway` must be within interface's subnet.",
                response_id: "DHCP_SERVER_GATEWAY_NOT_WITHIN_SUBNET"
            );
        }

        return $gateway;
    }

    /**
     * Provides extra validation for the `nonak` field.
     * @param bool $nonak The incoming `nonak` field value to validate.
     * @return bool The validated `nonak` field value to be assigned.
     * @throws ValidationError When `nonak` is `true` and a `failover_peerip` is assigned.
     */
    public function validate_nonak(bool $nonak) : bool {
        # Do not allow `nonak` to be enabled if a `failover_peerip` is set.
        if ($nonak and $this->failover_peerip->value) {
            throw new ValidationError(
                "DHCP server `nonak` cannot be enabled while a `failover_peerip` is assigned.",
                response_id: "DHCP_SERVER_NONAK_WITH_FAILOVER_PEERIP_NOT_ALLOWED"
            );
        }

        return $nonak;
    }

    /**
     * Provides extra validation for the `staticarp` field.
     * @param bool $staticarp The incoming `staticarp` field value to validate.
     * @return bool The validated `staticarp` field value to be assigned.
     * @throws ValidationError When `staticarp` is `true` but there are configured static mappings without IPs.
     */
    public function validate_staticarp(bool $staticarp) : bool {
        # Do not allow `staticarp` to be enabled if there are any configured static mappings without IPs
        # TODO: Replace this with a Model query when the DHCPServerStaticMapping model is developed
        foreach ($this->get_config("dhcpd/$this->id/staticmap", []) as $id => $static_mapping) {
            if (empty($map['ipaddr'])) {
                throw new ValidationError(
                    "DHCP server `staticarp` cannot be enabled while there are DHCP server static mappings ".
                        "configured without an assigned IP.",
                    response_id: "DHCP_SERVER_STATICARP_WITH_NO_IP_STATIC_MAPPINGS"
                );
            }
        }

        return $staticarp;
    }

    /**
     * Applies the pending DHCP server changes.
     */
    public function apply()
    {
        # Enable RRD graphing for DHCP if `statsgraph` changed
        if ($this->initial_object->statsgraph->value !== $this->statsgraph->value) {
            enable_rrd_graphing();
        }

        # Remove the existing dhcpd db files if the `failover_peerip` changed
        if ($this->initial_object->failover_peerip->value !== $this->failover_peerip->value) {
            mwexec("/bin/rm -rf /var/dhcpd/var/db/*");
        }

        (new DHCPServerApply())->apply();
    }
}
