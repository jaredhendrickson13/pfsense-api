<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallStatesSize extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/states/size";
    }

    protected function get() {
        return (new APIFirewallStatesSizeRead())->call();
    }

    protected function put() {
        return (new APIFirewallStatesSizeUpdate())->call();
    }
}