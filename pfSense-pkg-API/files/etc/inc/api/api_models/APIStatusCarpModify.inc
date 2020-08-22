<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIStatusCarpModify extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-carp"];
        $this->methods = ["POST"];
        $this->validators = [$this->validate_payload()];
    }

    private function validate_payload() {
        // Check if user specified enable value
        if (isset($this->initial_data['enable'])) {
            // Check if value is true or false
            if (boolval($this->initial_data['enable'])) {
                $enable = true;
            } else {
                $enable = false;
            }
            $this->validated_data["enable"] = $enable;
        }
        // Check if user specified maintenance mode value
        if (isset($this->initial_data['maintenance_mode'])) {
            // Check if value is true or false
            if (boolval($this->initial_data['maintenance_mode'])) {
                $mm_enable = true;
            } else {
                $mm_enable = false;
            }
            $this->validated_data["maintenance_mode"] = $mm_enable;
        }
    }

    public function action() {
        // Add our CARP settings
        $_SESSION["Username"] = $this->client->username;
        interfaces_carp_set_maintenancemode($this->validated_data["maintenance_mode"]);
        APITools\enable_carp($this->validated_data["enable"]);
        return APIResponse\get(0, $this->validated_data);
    }
}