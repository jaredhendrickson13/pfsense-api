<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIFirewallStatesSizeRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-statessummary"];
    }

    public function action() {

        // Check our maximum state table size
        if (isset($this->config["system"]["maximumstates"])) {
            $size_array["maximumstates"] = intval($this->config["system"]["maximumstates"]);
        } else {
            $size_array["maximumstates"] = intval(pfsense_default_state_size());
        }
        // Check our current state table size
        $size_array["currentstates"] = count(APITools\sort_state_table());
        // Check our default state table size
        $size_array["defaultmaximumstates"] = intval(pfsense_default_state_size());
        return APIResponse\get(0, $size_array);
    }
}