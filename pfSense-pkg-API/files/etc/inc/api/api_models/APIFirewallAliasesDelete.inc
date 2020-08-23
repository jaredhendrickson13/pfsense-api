<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallAliasesDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
    }

    public function action() {
        global $config;
        $_SESSION["Username"] = $this->client->username;    // Save our CLIENT ID to session data for logging
        $change_note = " Deleted firewall alias via API";    // Add a change note
        $del_conf = $config["aliases"]["alias"][$this->validated_data["id"]];    // Capture alias config before deleting
        unset($config["aliases"]["alias"][$this->validated_data["id"]]);    // Remove this alias from our configuration
        $config["aliases"]["alias"] = array_values($config["aliases"]["alias"]);    // Reindex array
        write_config(sprintf(gettext($change_note)));    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_conf);
    }
    
    public function validate_payload() {
        global $config;
        if (isset($this->initial_data['name'])) {
            $name = $this->initial_data['name'];
            $name = APITools\sanitize_str($name);

            // Check that alias is not in use in our configuration
            if (!APITools\alias_in_use($name)) {
                // Loop through our current config and find the index ID for our alias to delete
                $c_count = 0;    // Init loop counter
                foreach ($config["aliases"]["alias"] as $ce) {
                    // Check if this entry matches our requested value
                    if ($ce["name"] === $name) {
                        $del_index = $c_count;
                        break;
                    }
                    $c_count++;
                }

                if (is_numeric($del_index)) {
                    $this->validated_data["id"] = $del_index;
                } else {
                    $this->errors[] = APIResponse\get(4055);
                }

            } else {
                $this->errors[] = APIResponse\get(4051);
            }
        } else {
            $this->errors = APIResponse\get(4050);
        }

    }
}