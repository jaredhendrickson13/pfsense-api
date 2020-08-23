<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallRulesDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-rules-edit"];
    }

    public function action() {
        global $config;
        $_SESSION["Username"] = $this->client->username;    // Save our CLIENT ID to session data for logging
        $change_note = " Deleted firewall rule via API";    // Add a change note
        $del_rule = $config["filter"]["rule"][$this->validated_data["id"]];    // Save the rule we are deleting
        unset($config["filter"]["rule"][$this->validated_data["id"]]);    // Remove rule from our config
        APITools\sort_firewall_rules();    // Sort our firewall rules
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_rule);
    }
    
    public function validate_payload() {
        global $config;
        if (isset($this->initial_data['id'])) {
            // Check that our rule ID exists
            if (array_key_exists($this->initial_data["id"], $config["filter"]["rule"])) {
                $this->validated_data["id"] = $this->initial_data['id'];
            } else {
                $this->errors[] = APIResponse\get(4032);
            }
        } else {
            $this->errors[] = APIResponse\get(4031);
        }

    }
}