<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallRulesAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
    }

    public function action() {
        global $config;
        $next_rule_id = count($config["filter"]["rule"]);    // Save our next rule ID
        $_SESSION["Username"] = $this->client->username;    // Save our CLIENT ID to session data for logging
        $change_note = " Added firewall rule via API";    // Add a change note
        $config["filter"]["rule"][] = $this->validated_data;    // Write to our master config
        APITools\sort_firewall_rules($this->initial_data["top"], $next_rule_id);    // Sort our firewall rules
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $this->validated_data);
    }

    // TODO: break this down into smaller field specific validators
    public function validate_payload() {
        global $config;
        $user_created_msg = $this->client->username."@".$this->client->ip_address." (API)";
        $allowed_rule_types = array("pass", "block", "reject");    // Array of allowed rule types
        $allowed_ip_prot = array("inet", "inet6", "inet46");    // Array of allowed IP protocols
        $allowed_prot = array(
            "any",
            "tcp",
            "udp",
            "tcp/udp",
            "icmp",
            "esp",
            "ah",
            "gre",
            "ipv6",
            "igmp",
            "pim",
            "ospf",
            "carp",
            "pfsync"
        );
        // Array of allowed ICMP subtypes
        $icmp_subtypes = array(
            "althost",
            "dataconv",
            "echorep",
            "echoreq",
            "inforep",
            "inforeq",
            "ipv6-here",
            "ipv6-where",
            "maskrep",
            "maskreq",
            "mobredir",
            "mobregrep",
            "mobregreq",
            "paramprob",
            "photuris",
            "redir",
            "routeradv",
            "routersol",
            "skip",
            "squench",
            "timerep",
            "timereq",
            "timex",
            "trace",
            "unreach"
        );
        if (isset($this->initial_data['type'])) {
            $type = $this->initial_data['type'];
        } else {
            $this->errors[] = APIResponse\get(4033);
        }
        if (isset($this->initial_data['interface'])) {
            $interface = $this->initial_data['interface'];
            $interface = APITools\get_pfsense_if_id($interface);
        } else {
            $this->errors[] = APIResponse\get(4034);

        }
        if (isset($this->initial_data['ipprotocol'])) {
            $ipprotocol = $this->initial_data['ipprotocol'];
        } else {
            $this->errors[] = APIResponse\get(4035);
        }
        if (isset($this->initial_data['protocol'])) {
            $protocol = $this->initial_data['protocol'];
        } else {
            $this->errors[] = APIResponse\get(4036);
        }
        if (isset($this->initial_data['src'])) {
            $src = $this->initial_data['src'];
        } else {
            $this->errors[] = APIResponse\get(4037);
        }
        if (isset($this->initial_data['srcport'])) {
            $srcport = $this->initial_data['srcport'];
        }
        if (isset($this->initial_data['dst'])) {
            $dst = $this->initial_data['dst'];
        } else {
            $this->errors[] = APIResponse\get(4038);
        }
        if (isset($this->initial_data['dstport'])) {
            $dstport = $this->initial_data['dstport'];
        }
        if (isset($this->initial_data['icmptype'])) {
            $icmp_type = $this->initial_data['icmptype'];
            if (!is_array($icmp_type)) {
                $icmp_type = array($icmp_type);
            }
        }
        if (isset($this->initial_data['gateway'])) {
            $gateway = $this->initial_data['gateway'];
        }
        if (isset($this->initial_data['disabled'])) {
            $disabled = true;
        }
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
        }
        if (isset($this->initial_data['log'])) {
            $log = true;
        }
        if (isset($this->initial_data['top'])) {
            $this->initial_data['top'] = "top";
        }
        // INPUT VALIDATION/FORMATTING
        // Check that our required array/interface values are valid
        if (!in_array($type, $allowed_rule_types)) {
            $this->errors[] = APIResponse\get(4039);
        } elseif (!is_string($interface)) {
            $this->errors[] = APIResponse\get(4040);
        } elseif (!in_array($ipprotocol, $allowed_ip_prot)) {
            $this->errors[] = APIResponse\get(4041);
        } elseif (!in_array($protocol, $allowed_prot)) {
            $this->errors[] = APIResponse\get(4042);
        } elseif (isset($gateway) and !APITools\is_gateway($gateway)) {
            $this->errors[] = APIResponse\get(4043);
        }
        // Check if rule is not disabled
        if (!$disabled) {
            $this->validated_data["id"] = "";
        }
        // Check if logging is enabled
        if ($log) {
            $this->validated_data["log"] = "";
        }
        // Check if gateway was specified
        if (isset($gateway)) {
            $this->validated_data["gateway"] = $gateway;
        }
        $this->validated_data["type"] = $type;
        $this->validated_data["interface"] = $interface;
        $this->validated_data["ipprotocol"] = $ipprotocol;
        $this->validated_data["source"] = array();
        $this->validated_data["destination"] = array();
        $this->validated_data["descr"] = $descr;
        $this->validated_data["created"] = array("time" => time(), "username" => $user_created_msg);
        $this->validated_data["updated"] = $this->validated_data["created"];
        // Save our protocol if it is not 'any'
        if ($protocol !== "any") {
            $this->validated_data["protocol"] = $protocol;
        }
        // Add logging to config if enabled
        if ($log) {
            $this->validated_data["log"] = "";
        }
        // Check if our source and destination values are valid
        foreach (array("source" => $src, "destination" => $dst) as $dir => $val) {
            $dir_check = APITools\is_valid_rule_addr($val, $dir);
            if (!$dir_check["valid"] === true) {
                $input_err = true;
                if ($dir === "source") {
                    $this->errors[] = APIResponse\get(4044);
                } else {
                    $this->errors[] = APIResponse\get(4045);
                }
            } else {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            }
        }
        // Check if protocol calls for additional specifications
        if (in_array($protocol, array("tcp", "udp", "tcp/udp"))) {
            $port_req = true;
        } elseif ($protocol === "icmp") {
            // Check if user specified ICMP subtypes
            if (is_array($icmp_type)) {
                // Loop through each of our subtypes
                foreach ($icmp_type as $ict) {
                    if (!in_array($ict, $icmp_subtypes)) {
                        $this->errors[] = APIResponse\get(4046);
                    }
                }
                // Write our ICMP subtype config
                $this->validated_data["icmptype"] = implode(",", $icmp_type);
            }
        }
        // Check our src and dst port values if ports are required
        if ($port_req) {
            if (!isset($srcport) or !isset($dstport)) {
                $this->errors[] = APIResponse\get(4047);

            }
            foreach (array("source" => $srcport, "destination" => $dstport) as $dir => $val) {
                $val = str_replace("-", ":", $val);
                if (!is_port_or_range($val) and $val !== "any") {
                    $input_err = true;
                    if ($dir === "source") {
                        $this->errors[] = APIResponse\get(4048);

                    } else {
                        $this->errors[] = APIResponse\get(4049);
                    }
                } elseif ($val !== "any") {
                    $this->validated_data[$dir]["port"] = str_replace(":", "-", $val);;
                }
            }
        }
    }
}