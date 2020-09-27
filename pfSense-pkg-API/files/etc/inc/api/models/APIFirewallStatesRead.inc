<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallStatesRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-statessummary"];
    }

    public function action() {
        return APIResponse\get(0, APITools\sort_state_table());
    }
}