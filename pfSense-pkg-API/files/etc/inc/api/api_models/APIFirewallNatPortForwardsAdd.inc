<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallNatPortForwardsAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-nat-portforward-edit"];
    }

    public function action() {
        global $config;
        $_SESSION["Username"] = $this->client->username;
        $change_note = " Added NAT rule via API";    // Add a change note
        $config["nat"]["rule"][] = $this->validated_data;    // Write to our master config
        $next_rule_id = count($config["nat"]["rule"]);    // Save our next rule ID
        APITools\sort_nat_rules($this->initial_data["top"], $next_rule_id);    // Sort our nat rules
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        filter_configure();    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $this->validated_data);
    }

    // TODO: break this down into smaller field specific validators
    public function validate_payload() {
        global $config;
        $user_created_msg = $_SESSION["Username"]."@".$_SERVER["REMOTE_ADDR"]." (API)";    // Save the username and ip of client
        $allowed_nat_ref = ["enable", "disable", "purenat"];    // Save our allow NAT reflection types
        $allowed_prot = ["tcp", "udp", "tcp/udp", "icmp", "esp", "ah", "gre", "ipv6", "igmp", "pim", "ospf"];
        if (isset($this->initial_data['interface'])) {
            $interface = $this->initial_data['interface'];
            $interface = APITools\get_pfsense_if_id($interface);
        } else {
            $this->errors[] = APIResponse\get(4000);
        }
        if (isset($this->initial_data['protocol'])) {
            $protocol = $this->initial_data['protocol'];
        } else {
            $this->errors[] = APIResponse\get(4001);

        }
        if (isset($this->initial_data['target'])) {
            $localip = $this->initial_data['target'];
        } else {
            $this->errors[] = APIResponse\get(4002);
        }
        if (isset($this->initial_data['local-port'])) {
            $localport = $this->initial_data['local-port'];
        } else {
            $this->errors[] = APIResponse\get(4003);
        }
        if (isset($this->initial_data['src'])) {
            $src = $this->initial_data['src'];
        } else {
            $this->errors[] = APIResponse\get(4004);

        }
        if (isset($this->initial_data['srcport'])) {
            $srcport = $this->initial_data['srcport'];
        }
        if (isset($this->initial_data['dst'])) {
            $dst = $this->initial_data['dst'];
        } else {
            $this->errors[] = APIResponse\get(4005);

        }
        if (isset($this->initial_data['dstport'])) {
            $dstport = $this->initial_data['dstport'];
        }
        if (isset($this->initial_data['disabled'])) {
            if ($this->initial_data['disabled']) {
                $disabled = true;
            }
        }
        if (isset($this->initial_data['nordr'])) {
            if ($this->initial_data['nordr']) {
                $nordr = true;
            }
        }
        if (isset($this->initial_data['nosync'])) {
            if ($this->initial_data['nosync'] === true) {
                $nosync = true;
            }
        }
        if (isset($this->initial_data['top'])) {
            if ($this->initial_data['top']) {
                $this->initial_data = "top";
            }
        }
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
        }
        if (isset($this->initial_data['natreflection'])) {
            $natreflection = $this->initial_data['natreflection'];
        }
        
        // INPUT VALIDATION/FORMATTING
        // Check that our required array/interface values are valid
        if (!is_string($interface)) {
            $this->errors[] = APIResponse\get(4006);
        } elseif (!in_array($protocol, $allowed_prot)) {
            $this->errors[] = APIResponse\get(4007);
        } elseif (isset($natreflection) and !in_array($natreflection, $allowed_nat_ref)) {
            $this->errors[] = APIResponse\get(4008);
        } elseif (!is_ipaddrv4($localip) and !alias_in_use($localip)) {
            $this->errors[] = APIResponse\get(4009);
        } elseif (!is_port_or_range($localport)) {
            $this->errors[] = APIResponse\get(4010);
        }
        $this->validated_data = array();
        // Check if rule is disabled
        if ($disabled) {
            $this->validated_data["disabled"] = "";
        }
        // Check if pfsync is disabled
        if ($nosync) {
            $this->validated_data["nosync"] = "";
        }
        // Check if RDR is disabled is disabled
        if ($nordr) {
            $this->validated_data["nordr"] = "";
        }
        // Check if user specified NAT reflection
        if ($natreflection) {
            $this->validated_data["natreflection"] = $natreflection;
        }
        $this->validated_data["interface"] = $interface;
        $this->validated_data["protocol"] = $protocol;
        $this->validated_data["source"] = array();
        $this->validated_data["destination"] = array();
        $this->validated_data["target"] = $localip;
        $this->validated_data["local-port"] = $localport;
        $this->validated_data["descr"] = $descr;
        $this->validated_data["associated-rule-id"] = "pass";
        $this->validated_data["created"] = array("time" => time(), "username" => $user_created_msg);
        $this->validated_data["updated"] = $this->validated_data["created"];
        // Check if our source and destination values are valid
        foreach (array("source" => $src, "destination" => $dst) as $dir => $val) {
            $dir_check = APITools\is_valid_rule_addr($val, $dir);
            if (!$dir_check["valid"]) {
                if ($dir === "source") {
                    $this->errors[] = APIResponse\get(4011);
                } else {
                    $this->errors[] = APIResponse\get(4012);

                }
            } else {
                $this->validated_data = array_merge($this->validated_data, $dir_check["data"]);
            }
        }
        // Check if protocol calls for additional specifications
        if (in_array($protocol, array("tcp", "udp", "tcp/udp"))) {
            $port_req = true;
        }
        // Check our src and dst port values if ports are required
        if ($port_req) {
            foreach (array("source" => $srcport, "destination" => $dstport) as $dir => $val) {
                $val = str_replace("-", ":", $val);
                if (!is_port_or_range($val) and $val !== "any") {
                    if ($dir === "source") {
                        $this->errors[] = APIResponse\get(4013);

                    } else {
                        $this->errors[] = APIResponse\get(4014);
                    }
                } elseif ($val !== "any") {
                    $this->validated_data[$dir]["port"] = str_replace(":", "-", $val);
                }
            }
        }
    }
}