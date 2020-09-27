<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallVirtualIPDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->change_note = "Deleted virtual IP via API";
    }

    public function action() {
        $del_vip = $this->config["virtualip"]["vip"][$this->id];
        APITools\bring_down_virtual_ip($del_vip, $this->id);
        $this->write_config();
        send_event("filter reload");
        return APIResponse\get(0, $del_vip);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['id'])) {
            # Check that our rule ID exists
            if (array_key_exists($this->initial_data["id"], $this->config["virtualip"]["vip"])) {
                $this->id = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4018);
            }
        } else {
            $this->errors[] = APIResponse\get(4017);
        }
    }
}