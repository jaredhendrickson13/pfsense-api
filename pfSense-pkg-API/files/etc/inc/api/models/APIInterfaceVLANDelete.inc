<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIInterfaceVLANDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-interfaces-vlan-edit"];
        $this->change_note = "Deleted interface VLAN via API";

    }

    public function action() {
        $del_ent = $this->config["vlans"]["vlan"][$this->validated_data["id"]];    // Save our deleted VLAN
        pfSense_interface_destroy($this->config["vlans"]["vlan"][$this->validated_data["id"]]["vlanif"]);    // delete our VLAN on the backend
        unset($this->config["vlans"]["vlan"][$this->validated_data["id"]]);    // Remove our VLAN configuration
        $this->write_config();
        return APIResponse\get(0, $del_ent);
    }

    public function validate_payload() {
        $curr_vlans = $this->config["vlans"]["vlan"];    // Save our current VLANs
        if (isset($this->initial_data['vlanif'])) {
            $this->validated_data["vlanif"] = $this->initial_data['vlanif'];
        }
        elseif (isset($this->initial_data['id'])) {
            $this->validated_data["id"] = $this->initial_data['id'];
        } else {
            $this->errors[] = APIResponse\get(3048);
        }
        // Ensure we have a vlanif and id regardless of which input selector was provided
        if (isset($this->validated_data["vlanif"])) {
            foreach ($curr_vlans as $ind => $cve) {
                if ($this->validated_data["vlanif"] === $cve["vlanif"]) {
                    $this->validated_data["id"] = $ind;
                    break;
                }
            }
        } else {
            $this->validated_data["vlanif"] = $curr_vlans[$this->validated_data["id"]]["vlanif"];
        }
        // Check that our interface is not in use currently
        if (convert_real_interface_to_friendly_interface_name($this->validated_data["vlanif"])) {
            $this->errors[] = APIResponse\get(3049);
        }
        if (empty($this->config["vlans"]["vlan"][$this->validated_data["id"]])) {
            $this->errors[] = APIResponse\get(3050);
        }
    }
}