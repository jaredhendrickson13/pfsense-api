<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallVirtualIPs extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-firewall-virtualipaddresses"];
    }

    public function action() {
        global $config;
        // Check that we have a virtual IP configuration
        if (!empty($config["virtualip"]["vip"])) {
            $vip_array = $config["virtualip"]["vip"];
        } else {
            $vip_array = [];
        }
        return APIResponse\get(0, $vip_array);
    }
}