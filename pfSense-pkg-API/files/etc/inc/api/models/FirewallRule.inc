<?php

namespace api\models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;
use API\Fields\FilterAddressField;
use API\Fields\ForeignModelField;
use API\Fields\InterfaceField;
use API\Fields\StringField;
use API\Responses\ValidationError;

class FirewallRule extends Model
{
    public StringField $type;
    public InterfaceField $interface;
    public StringField $ipprotocol;
    public StringField $protocol;
    public StringField $icmptype;
    public FilterAddressField $source;
    public FilterAddressField $destination;
    public ForeignModelField $gateway;
    public StringField $descr;
    public BooleanField $disabled;
    public BooleanField $log;
    public StringField $statetype;
    public StringField $tcp_flags_set;
    public StringField $tcp_flags_out_of;
    public BooleanField $tcp_flags_any;
    public BooleanField $floating;
    public BooleanField $quick;
    public StringField $direction;


    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Set model attributes
        $this->config_path = "filter/rule";
        $this->many = true;

        # Set model fields
        $this->type = new StringField(
            required: true,
            choices: ["pass", "block", "reject"],
            help_text: "The action to take against traffic that matches this rule."
        );
        $this->interface = new InterfaceField(
            required: true,
            allow_interface_groups: true,
            allow_ipsec_interface: true,
            allow_openvpn_interface: true,
            many: true,
            help_text: "The interface where packets must originate to match this rule."
        );
        $this->ipprotocol = new StringField(
            required: true,
            choices: ["inet", "inet6", "inet46"],
            help_text: "The IP version(s) this rule applies to."
        );
        $this->protocol = new StringField(
            default: null,
            choices: [
                "any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah",
                "gre", "ipv6", "igmp", "pim", "ospf", "carp", "pfsync"
            ],
            allow_null: true,
            help_text: "The IP/transport protocol this rule should match."
        );
        $this->icmptype = new StringField(
            default: "any",
            choices: [
                "althost", "dataconv", "echorep", "echoreq", "inforep", "inforeq", "ipv6-here", "ipv6-where",
                "maskrep", "maskreq", "mobredir", "mobregrep", "mobregreq", "paramprob", "photuris", "redir",
                "routeradv", "routersol", "skip", "squench", "timerep", "timereq", "timex", "trace", "unreach"
            ],
            many: true,
            conditions: ["protocol" => "icmp"],
            help_text: "Th ICMP subtypes this rule applies to. This field is only applicable when ".
                "`ipprotocol` is `inet` and `protocol` is `icmp`."

        );
        $this->source = new FilterAddressField(
            required: true,
            help_text: "The source address this rule applies to."
        );
        $this->destination = new FilterAddressField(
            required: true,
            help_text: "The destination address this rule applies to."
        );
        $this->gateway = new ForeignModelField(
            model_name: "RoutingGateway",
            model_field: "name",
            default: null,
            allow_null: true,
            help_text: "The gateway traffic matching this rule will be routed to. Set to `null` to use default."
        );
        $this->statetype = new StringField(
            default: "keep state",
            choices: ["keep state", "sloppy state", "synproxy state", "none"],
            help_text: "The state mechanism to use for this firewall rule."
        );
        $this->tcp_flags_set = new StringField(
            default: null,
            choices: ["fin", "syn", "rst", "psh", "ack", "urg", "ece", "cwr"],
            allow_null: true,
            many: true,
            internal_name: "tcpflags1",
            conditions: ["tcp_flags_any" => false, "!tcp_flags_out_of" => null],
            help_text: "The TCP flags that must be set for this rule to match."
        );
        $this->tcp_flags_out_of = new StringField(
            default: null,
            choices: ["fin", "syn", "rst", "psh", "ack", "urg", "ece", "cwr"],
            allow_null: true,
            many: true,
            internal_name: "tcpflags2",
            conditions: ["tcp_flags_any" => false, "!tcp_flags_set" => null],
            help_text: "The TCP flags that can be set for this rule to match."
        );
        $this->tcp_flags_any = new BooleanField(
            default: false,
            internal_name: "tcpflags_any",
            help_text: "Allow any TCP flags."
        );
        $this->descr = new StringField(
            default: "",
            allow_empty: true,
            help_text: "A description detailing the purpose or justification of this firewall rule."
        );
        $this->disabled = new BooleanField(
            default: false,
            help_text: "Enable or disable this firewall rule."
        );
        $this->log = new BooleanField(
            default: false,
            help_text: "Enable or disable logging of traffic that matches this rule."
        );
        $this->floating = new BooleanField(
            default: false,
            editable: false,
            indicates_true: "yes",
            help_text: "Mark this rule is a floating firewall rule."
        );
        $this->quick = new BooleanField(
            default: false,
            indicates_true: "yes",
            conditions: ["floating" => true],
            help_text: "Apply this action to traffic that matches this rule immediately. This field only applies ".
                "to floating firewall rules."
        );
        $this->direction = new StringField(
            default: "any",
            choices: ["any", "in", "out"],
            conditions: ["floating" => true],
            help_text: "The direction of traffic this firewall rule applies to. This field only applies to ".
                "floating firewall rules."
        );
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Adds extra validation to the `interface` field.
     * @param string $interface The incoming value to be validated.
     * @return string The validated value to be set.
     */
    public function validate_interface(string $interface) : string {
        # Do not allow more than one interface to be assigned if this is not a floating rule
        if (!$this->floating->value and count($this->interface->value) > 1) {
            throw new ValidationError(
                message: "Field `interface` cannot contain more than one value unless `floating` is enabled.",
                response_id: "FIREWALL_RULE_MULTIPLE_INTERFACE_WITHOUT_FLOATING"
            );
        }

        return $interface;
    }

    /**
     * Adds extra validation to the `statetype` field.
     * @param string $statetype The incoming value to be validated.
     * @return string The validated value to be set.
     */
    public function validate_statetype(string $statetype) : string {
        # Do not allow the `synproxy state` value to be set if `protocol` is not `tcp`
        if ($statetype === "synproxy state" and $this->protocol->value !== "tcp") {
            throw new ValidationError(
                message: "Field `statetype` cannot be set to `synproxy state` unless `protocol` is set to `tcp`.",
                response_id: "FIREWALL_RULE_SYNPROXY_STATE_TYPE_WITH_NON_TCP_PROTOCOL"
            );
        }

        # Do not allow the `synproxy state` value to be set if a `gateway` is set
        if ($statetype === "synproxy state" and $this->gateway->value) {
            throw new ValidationError(
                message: "Field `statetype` cannot be set to `synproxy state` when a `gateway` value is set.",
                response_id: "FIREWALL_RULE_SYNPROXY_STATE_TYPE_WITH_GATEWAY"
            );
        }

        return $statetype;
    }

    /**
     * Adds extra validation to the `tcp_flags_set` field.
     * @param string $tcp_flags_set The incoming value to be validated.
     * @return string The validated value to be set.
     */
    public function validate_tcp_flags_set(string $tcp_flags_set) : string {
        # Ensure this value is also present in `tcp_flags_out_of`
        if (!in_array($tcp_flags_set, $this->tcp_flags_out_of->value)) {
            throw new ValidationError(
                message: "Field `tcp_flags_set` value `$tcp_flags_set` is not present in `tcp_flags_out_of`.",
                response_id: "FIREWALL_RULE_TCP_FLAGS_SET_NOT_IN_TCP_FLAGS_OUT_OF"
            );
        }

        return $tcp_flags_set;
    }
}