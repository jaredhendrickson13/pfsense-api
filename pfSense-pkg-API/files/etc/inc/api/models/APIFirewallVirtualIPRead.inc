<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallVirtualIPRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-virtualipaddresses"];
    }

    public function action() {

        // Check that we have a virtual IP configuration
        if (!empty($this->config["virtualip"]["vip"])) {
            $vip_array = [];
            # Loop through each virtual IP to include it's config ID
            foreach ($this->config["virtualip"]["vip"] as $id=>$vip) {
                $vip["id"] = $id;
                $vip_array[] = $vip;
            }
        } else {
            $vip_array = [];
        }
        return APIResponse\get(0, $vip_array);
    }
}