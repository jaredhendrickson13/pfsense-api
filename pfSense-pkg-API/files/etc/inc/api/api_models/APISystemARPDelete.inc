<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemARPDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-diagnostics-arptable"];
    }

    public function action() {
        $del_ent = APITools\get_arp_entry("ip", $this->validated_data["ip"]);    // Save our deleted ARP config
        exec("arp -d ".$this->validated_data["ip"], $arp_del);    // Delete our ARP address
        return APIResponse\get(0, $del_ent);
    }
    
    public function validate_payload() {
        if (isset($this->initial_data['ip'])) {
            $this->validated_data["ip"] = $this->initial_data['ip'];
            $this->validated_data["ip"] = trim($this->validated_data["ip"]);
        } else {
            $this->errors[] = APIResponse\get(1006);
        }
    }
}