<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallNat extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->validators = [];
    }

    public function action() {

        // Check that we have a NAT configuration
        if (!empty($this->config["nat"])) {
            $this->validated_data = $this->config["nat"];
        }
        return APIResponse\get(0, $this->validated_data);
    }
}