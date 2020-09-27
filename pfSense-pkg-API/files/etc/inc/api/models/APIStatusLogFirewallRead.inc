<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIStatusLogFirewallRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-logs-firewall"];
    }

    public function action() {
        $log = file_get_contents("/var/log/filter.log");
        $log = explode("\n", $log);
        $log = array_filter($log);
        unset($log[count($log) - 1]);
        return APIResponse\get(0, $log);
    }
}