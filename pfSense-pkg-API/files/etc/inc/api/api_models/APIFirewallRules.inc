<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallRules extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
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