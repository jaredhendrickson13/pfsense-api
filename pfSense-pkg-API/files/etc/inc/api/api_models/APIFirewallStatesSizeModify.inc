<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/api_models/APIFirewallStatesSize.inc");


class APIFirewallStatesSizeModify extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system-advanced-firewall"];
        $this->change_note = "Modified firewall state table size via API";
    }

    public function action() {

        // Write our state table size
        if ($this->validated_data["maximumstates"] === "default") {
            unset($this->config["system"]["maximumstates"]);
        } else {
            $this->config["system"]["maximumstates"] = $this->validated_data["maximumstates"];
        }
        $this->write_config();    // Apply our configuration change
        filter_configure();    // Update our firewall filter
        return APIResponse\get(0, (new APIFirewallStatesSize())->action()["data"]);
    }

    public function validate_payload() {
        // Check if we set a maximumstates size
        if (isset($this->initial_data["maximumstates"])) {
            // Check that our state size is valid
            if (is_numeric($this->initial_data["maximumstates"]) and (10 <= intval($this->initial_data["maximumstates"]))) {
                $this->validated_data["maximumstates"] = $this->initial_data["maximumstates"];
            } elseif ($this->initial_data["maximumstates"] === "default") {
                $this->validated_data["maximumstates"] = $this->initial_data["maximumstates"];
            } else {
                $this->errors[] = APIResponse\get(4073);
            }
        } else {
            $this->errors[] = APIResponse\get(4072);
        }
    }
}