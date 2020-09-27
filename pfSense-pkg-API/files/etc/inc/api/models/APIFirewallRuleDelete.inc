<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallRuleDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
        $this->change_note = "Deleted firewall rule via API";
    }

    public function action() {
        $del_rule = $this->config["filter"]["rule"][$this->id];    // Save the rule we are deleting
        unset($this->config["filter"]["rule"][$this->id]);    // Remove rule from our config
        APITools\sort_firewall_rules();    // Sort our firewall rules
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        # Check for our required 'tracker' payload value
        if (isset($this->initial_data['tracker'])) {
            # Loop through each rule and check if our tracker matches
            foreach ($this->config["filter"]["rule"] as $id=>$rule) {
                if (intval($rule["tracker"]) === intval($this->initial_data['tracker'])) {
                    $this->id = $id;
                    break;
                }
            }

            # Return not found error if no match was found
            if (!is_int($this->id)) {
                $this->errors[] = APIResponse\get(4032);
            }
        } else {
            $this->errors[] = APIResponse\get(4031);
        }
    }
}