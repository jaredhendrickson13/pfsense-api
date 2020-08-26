<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallAliasesDelete extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-firewall-aliases-edit"];
        $this->change_note = "Deleted firewall alias via API";
    }

    public function action() {
        $del_conf = $this->config["aliases"]["alias"][$this->validated_data["id"]];    // Capture alias config before deleting
        unset($this->config["aliases"]["alias"][$this->validated_data["id"]]);    // Remove this alias from our configuration
        $this->config["aliases"]["alias"] = array_values($this->config["aliases"]["alias"]);    // Reindex array
        $this->write_config();    // Apply our configuration change
        send_event("filter reload");    // Ensure our firewall filter is reloaded
        return APIResponse\get(0, $del_conf);
    }
    
    public function validate_payload() {

        if (isset($this->initial_data['name'])) {
            $name = $this->initial_data['name'];
            $name = APITools\sanitize_str($name);

            // Check that alias is not in use in our configuration
            if (!APITools\alias_in_use($name)) {
                // Loop through our current config and find the index ID for our alias to delete
                $c_count = 0;    // Init loop counter
                foreach ($this->config["aliases"]["alias"] as $ce) {
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