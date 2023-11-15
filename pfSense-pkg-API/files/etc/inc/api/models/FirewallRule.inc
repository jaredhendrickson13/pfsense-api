<?php

namespace api\models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\ForeignModelField;
use API\Fields\InterfaceField;
use API\Fields\StringField;

class FirewallRule extends Model
{
    public StringField $type;
    public InterfaceField $interface;
    public StringField $ipprotocol;
    public StringField $protocol;
    public StringField $icmptype;
    public ForeignModelField $gateway;
    public StringField $sched;

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
            help_text: "The interface where packets must originate to match this rule."
        );
        $this->ipprotocol = new StringField(
            required: true,
            choices: ["inet", "inet6", "inet46"],
            help_text: "The IP version(s) this rule applies to."
        );
        $this->protocol = new StringField(
            required: true,
            choices: [
                "any", "tcp", "udp", "tcp/udp", "icmp", "esp", "ah",
                "gre", "ipv6", "igmp", "pim", "ospf", "carp", "pfsync"
            ],
            help_text: "The IP/transport protocol this rule should match."
        );
        $this->icmptype = new StringField(
            default: "any",
            choices: [
                "any", "althost", "dataconv", "echorep", "echoreq", "inforep", "inforeq", "ipv6-here", "ipv6-where",
                "maskrep", "maskreq", "mobredir", "mobregrep", "mobregreq", "paramprob", "photuris", "redir",
                "routeradv", "routersol", "skip", "squench", "timerep", "timereq", "timex", "trace", "unreach"
            ],
            many: true,
            help_text: "The IPv4 ICMP subtypes this rule applies to. This field is only applicable when ".
                "`ipprotocol` is `inet` and `protocol` is `icmp`."

        );
        parent::__construct($id, $representation_data, $client);
    }
}