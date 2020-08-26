<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallNatPortForwardsDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-nat-portforward-edit"];
        $this->change_note = "Deleted NAT rule via API";
    }

    public function action() {
        $del_rule = $this->config["nat"]["rule"][$this->validated_data["id"]];    // Save the rule we are deleting
        unset($this->config["nat"]["rule"][$this->validated_data["id"]]);    // Remove rule from our config
        APITools\sort_nat_rules();    // Sort our NAT rules
        $this->write_config();    // Apply our configuration change
        filter_configure();    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {

        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if (array_key_exists($this->initial_data['id'], $this->config["nat"]["rule"])) {
                $this->validated_data["id"] = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4016);
            }
        } else {
            $this->errors[] = APIResponse\get(4015);
        }
    }
}