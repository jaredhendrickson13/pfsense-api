<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallRuleRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-firewall-rules"];
    }

    public function action() {

        // Check that we have a configuration
        if (!empty($this->config["filter"]["rule"])) {
            $rule_array = $this->config["filter"]["rule"];
        } else {
            $rule_array = [];
        }
        return APIResponse\get(0, $rule_array);
    }
}