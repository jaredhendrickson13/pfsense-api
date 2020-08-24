<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallVirtualIPsAdd extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Added virtual IP via API";
    }

    public function action() {
        $this->config["virtualip"]["vip"][] = $this->validated_data;    // Write to our master config
        $this->write_config();    // Apply our configuration change
        APITools\apply_virtual_ip($this->validated_data);    // Apply our backend changes with our new configuration
        return APIResponse\get(0, $this->validated_data);
    }
    
    public function validate_payload() {

        $allowed_vip_modes = array("ipalias", "carp", "proxyarp", "other");    // Save our allowed vip modes in arra
        if (isset($this->initial_data['mode'])) {
            $mode = strtolower($this->initial_data['mode']);
        } else {
            $this->errors[] = APIResponse\get(4019);
        }
        if (isset($this->initial_data['interface'])) {
            $interface = $this->initial_data['interface'];
            $interface = APITools\get_pfsense_if_id($interface);
        } else {
            $this->errors[] = APIResponse\get(4020);
        }
        if (isset($this->initial_data['subnet'])) {
            $subnet = $this->initial_data['subnet'];
            // If a single IPv4 or IPv6, append the subnet mask for one address
            if (is_ipaddrv4($subnet)) {
                $subnet = $subnet."/32";
            } elseif (is_ipaddrv6($subnet)) {
                $subnet = $subnet."/128";
            }
        } else {
            $this->errors[] = APIResponse\get(4021);
        }
        if (isset($this->initial_data['descr'])) {
            $descr = $this->initial_data['descr'];
        } else {
            $descr = "";
        }
        if (isset($this->initial_data['noexpand'])) {
            $noexpand = true;
        }
        if (!in_array($mode, $allowed_vip_modes)) {
            $this->errors[] = APIResponse\get(4023);
        }
        // CARP required attributes
        if ($mode === "carp" and isset($this->initial_data['vhid'])) {
            $vhid = $this->initial_data['vhid'];
        }
        if ($mode === "carp" and isset($this->initial_data['advskew'])) {
            $advskew = intval($this->initial_data['advskew']);
        } else {
            $advskew = 0;    // Default skew to 0
        }
        if ($mode === "carp" and isset($this->initial_data['advbase'])) {
            $advbase = intval($this->initial_data['advbase']);
        } else {
            $advbase = 1;    // Default base to 0
        }
        if ($mode === "carp" and isset($this->initial_data['password'])) {
            $password = $this->initial_data['password'];
        } else {
            $this->errors[] = APIResponse\get(4022);
        }
        // INPUT VALIDATION/FORMATTING// Check that our required array/interface values are valid
        if (!is_string($interface)) {
            $this->errors[] = APIResponse\get(4024);

        } elseif (!is_subnet($subnet)) {
            $this->errors[] = APIResponse\get(4025);
        }
        // Split our subnet into an address and subnet mask
        $subnet_split = explode("/", $subnet);
        $subnet = $subnet_split[0];
        $subnet_bits = $subnet_split[1];
        // Check that our subnet is not used elsewhere
        if (APITools\is_ip_in_use($subnet)) {
            $this->errors[] = APIResponse\get(4026);
        }
        // Check if VHID was specified
        if (isset($vhid)) {
            if (vhid_exists($vhid)) {
                $this->errors[] = APIResponse\get(4027);

            } elseif (1 > $vhid or $vhid > 255) {
                $this->errors[] = APIResponse\get(4028);

            }
        } elseif ($mode === "carp" and !isset($vhid)) {
            $vhid = APITools\next_vhid();    // Pull our next available VHID
        }
        // Check if advertisement base was specified
        if (isset($advbase)) {
            if (1 > $advbase or $advbase > 254) {
                $this->errors[] = APIResponse\get(4029);

            }
        }
        // Check if advertisement skew was specified
        if (isset($advskew)) {
            if (0 > $advskew or $advskew > 254) {
                $this->errors[] = APIResponse\get(4030);

            }
        }
        // Initialize our virtual IP configuration array
        if (!is_array($this->config["virtualip"]["vip"])) {
            $this->config["virtualip"] = array("vip" => []);
        }
        // Populate our new virtual IP entry
        $this->validated_data["mode"] = $mode;
        $this->validated_data["interface"] = $interface;
        $this->validated_data["type"] = "network";
        $this->validated_data["subnet"] = $subnet;
        $this->validated_data["subnet_bits"] = $subnet_bits;
        $this->validated_data["descr"] = $descr;
        // Values specific to CARP
        if ($mode === "carp") {
            $this->validated_data["vhid"] = $vhid;
            $this->validated_data["advbase"] = $advbase;
            $this->validated_data["advskew"] = $advskew;
            $this->validated_data["password"] = $password;
            $this->validated_data["uniqid"] = uniqid();
        }
        // Values specific to Proxy ARP and other
        if (in_array($mode, array("proxyarp", "other")) and $noexpand) {
            $this->validated_data["noexpand"] = "";
        }
    }
}