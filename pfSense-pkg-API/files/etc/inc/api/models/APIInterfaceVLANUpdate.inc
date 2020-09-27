<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfaceVLANUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-vlan-edit"];
        $this->change_note = "Modified VLAN interface via API";

    }

    public function action() {
        $vlan_ent =& $this->config["vlans"]["vlan"][$this->initial_data["id"]];    // Get current VLAN config
        $vlan_ent = array_replace($vlan_ent, $this->validated_data);    // Update our VLAN config
        $vlan_ent["vlanif"] = $vlan_ent["if"].".".$vlan_ent["tag"];    // Update VLAN iface ID

        pfSense_interface_destroy($this->initial_data["vlanif"]);
        interface_vlan_configure($vlan_ent);
        // Check if we need to reassign an interface
        $assigned_if = APITools\get_pfsense_if_id($this->initial_data["vlanif"]);
        if (!empty($assigned_if)) {
            $this->config['interfaces'][$assigned_if]['if'] = $vlan_ent["vlanif"];    // Write interface config
            $this->write_config();
            interface_configure($assigned_if);    // Configure our assigned interface
        }
        $this->write_config();
        return APIResponse\get(0, $vlan_ent);
    }

    public function validate_payload() {
        $if_list = get_interface_list();    // Get our interfaces list
        $lagg_list = get_lagg_interface_list();    // Get our lagg interfaces list
        $avail_ifs = $if_list + $lagg_list;    // Combine the two lists
        // Remove LAGG interface members as they cannot be assigned VLANs
        foreach ($lagg_list as $lagg_if => $lagg) {
            $lagg_members = explode(',', $lagg['members']);
            foreach ($lagg_members as $lagm) {
                if (isset($avail_ifs[$lagm])) {
                    unset($avail_ifs[$lagm]);
                }
            }
        }
        if (!isset($this->initial_data['vlanif'])) {
            $this->errors[] = APIResponse\get(3048);
        }
        if (isset($this->initial_data['if'])) {
            $this->validated_data["if"] = $this->initial_data['if'];
        }
        if (isset($this->initial_data['tag'])) {
            $this->validated_data["tag"] = $this->initial_data['tag'];
            $this->validated_data["tag"] = intval(trim($this->validated_data["tag"]));
            $str_tag = strval($this->validated_data["tag"]);
        }
        if (isset($this->initial_data['pcp'])) {
            $this->validated_data["pcp"] = $this->initial_data['pcp'];
            $this->validated_data["pcp"] = intval(trim($this->validated_data["pcp"]));
        }
        if (isset($this->initial_data['descr'])) {
            $this->validated_data["descr"] = $this->initial_data['descr'];
        }
        // Ensure we have a vlanif and id regardless of which input selector was provided
        if (isset($this->initial_data["vlanif"])) {
            foreach ($this->config["vlans"]["vlan"] as $ind => $cve) {
                if ($this->initial_data["vlanif"] === $cve["vlanif"]) {
                    $this->initial_data["id"] = $ind;
                    break;
                }
            }
        } else {
            $this->initial_data["vlanif"] = $this->config["vlans"]["vlan"][$this->initial_data["id"]]["vlanif"];
        }
        // Input validation
        // Check if our parent interface exists
        if (!array_key_exists($this->initial_data["id"], $this->config["vlans"]["vlan"])) {
            $this->errors[] = APIResponse\get(3050);
        } elseif (isset($this->validated_data["if"]) and !does_interface_exist($this->validated_data["if"])) {
            $this->errors[] = APIResponse\get(3051);
        } elseif (isset($this->validated_data["tag"]) and (1 > $this->validated_data["tag"] or $this->validated_data["tag"] > 4096)) {
            $this->errors[] = APIResponse\get(3052);
        } elseif (isset($this->validated_data["pcp"]) and (0 > $this->validated_data["pcp"] or $this->validated_data["pcp"] > 7)) {
            $this->errors[] = APIResponse\get(3053);
        }
        // Check if our VLAN is already in use
        if (is_array($this->config["vlans"]["vlan"])) {
            if (isset($this->validated_data["if"]) and isset($this->validated_data["tag"])) {
                foreach ($this->config["vlans"]["vlan"] as $vle) {
                    if ($this->validated_data["if"] === $vle["if"] and $str_tag === $vle["tag"]) {
                        $this->errors[] = APIResponse\get(3054);

                    }
                }
            }
        }
    }
}