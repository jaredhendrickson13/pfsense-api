<?php

namespace API\Models;

use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\IntegerField;
use API\Fields\StringField;
use API\Responses\ValidationError;
use API\Validators\IPAddressValidator;
use API\Validators\MACAddressValidator;

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
    public function __construct()
    {
        # Define Model attributes
        $this->config_path = "dhcpd";
        $this->many = true;
        $this->update_strategy = "merge";

        # Define Model fields
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
            default: 7200,
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
            many_maximum: 1,    # TODO: allow `many` fields to save internally as array to allow more than 1 here
            maximum_length: 15,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The DNS servers to provide via DHCP. Leave empty to default to system nameservers."
        );
        $this->winsserver = new StringField(
            default: [],
            allow_empty: true,
            many_maximum: 1,    # TODO: allow `many` fields to save internally as array to allow more than 1 here
            maximum_length: 15,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: false)],
            help_text: "The WINS servers to provide via DHCP."
        );
        $this->ntpserver = new StringField(
            default: [],
            allow_empty: true,
            many_maximum: 1,    # TODO: allow `many` fields to save internally as array to allow more than 1 here
            maximum_length: 15,
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
            allow_null: true,
            help_text: "Define how to handle unknown clients requesting DHCP leases. When set to `null`, any DHCP ".
                "client will get an IP address within this scope/range on this interface. If set to `enabled`, ".
                "any DHCP client with a MAC address listed in a static mapping on any scope(s)/interface(s) will ".
                "get an IP address. If set to `class`, only MAC addresses listed in static mappings on this interface ".
                "will get an IP address within this scope/range."
        );
    }

    /**
     * Provides extra validation for the `enable` field.
     * @param bool $enable The incoming `enable` field value to validate.
     * @return bool The validated `enable` field value to be assigned.
     * @throws ValidationError When `enable` is `true` but interface associated with `id` does not have a static IP.
     */
    public function validate_enable(bool $enable) : bool {
        # Get the interface associated with this DHCP server
        $interface = NetworkInterface::query(["id" => $this->id])->first();

        # Do not allow the DHCP server to be enabled if the interface does not have a static IP address assigned
        if ($this->enable->value and $interface->typev4->value !== "static") {
            throw new ValidationError(
                message: "DHCP server cannot be enabled because interface `{$this->id}` does not have a static IPv4 ".
                    "address assigned.",
                response_id: "DHCP_SERVER_CANNOT_ENABLE_WITHOUT_STATIC_IPV4"
            );
        }

        return $enable;
    }

    /**
     * Provides extra validation for the `nonak` field.
     * @param bool $nonak The incoming `nonak` field value to validate.
     * @return bool The validated `nonak` field value to be assigned.
     * @throws ValidationError When `nonak` is `true` and a `failover_peerip` is assigned.
     */
    public function validate_nonak(bool $nonak) : bool {
        # Do not allow `nonak` to be enabled if a `failover_peerip` is set.
        if ($this->nonak->value and $this->failover_peerip->value) {
            throw new ValidationError(
                "DHCP server `nonak` cannot be enabled while a `failover_peerip` is assigned.",
                response_id: "DHCP_SERVER_NONAK_WITH_FAILOVER_PEERIP_NOT_ALLOWED"
            );
        }

        return $nonak;
    }
}