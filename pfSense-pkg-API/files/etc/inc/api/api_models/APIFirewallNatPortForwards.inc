<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIFirewallNatPortForwards extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-firewall-nat-portforward"];
    }

    public function action() {
        global $config;
        // Check that we have a virtual IP configuration
        if (!empty($config["nat"]["rule"])) {
            $this->validated_data = $config["nat"]["rule"];
        }
        return APIResponse\get(0, $this->validated_data);
    }
}