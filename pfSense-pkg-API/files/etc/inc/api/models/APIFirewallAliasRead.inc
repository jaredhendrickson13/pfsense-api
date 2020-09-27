<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallAliasRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-aliases"];
    }

    public function action() {

        // Check that we have a configuration
        if (!empty($this->config["aliases"]["alias"])) {
            $alias_array = $this->config["aliases"]["alias"];
        } else {
            $alias_array = [];
        }
        return APIResponse\get(0, $alias_array);
    }
}