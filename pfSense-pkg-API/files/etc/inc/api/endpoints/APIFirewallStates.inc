<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallStates extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/states";
    }

    protected function get() {
        return (new APIFirewallStatesRead())->call();
    }
}