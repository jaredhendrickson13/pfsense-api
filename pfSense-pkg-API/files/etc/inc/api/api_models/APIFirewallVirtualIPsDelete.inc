<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallVirtualIPsDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Deleted virtual IP via API";
    }

    public function action() {
        $del_vip = $this->config["virtualip"]["vip"][$this->validated_data["id"]];    // Save the virtual IP we are deleting
        APITools\bring_down_virtual_ip($del_vip, $this->validated_data["id"]);    // Bring down v IP and delete it
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_vip);
    }
    
    public function validate_payload() {

        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if (array_key_exists($this->initial_data["id"], $this->config["virtualip"]["vip"])) {
                $this->validated_data["id"] = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4018);
            }
        } else {
            $this->errors[] = APIResponse\get(4017);
        }
    }
}