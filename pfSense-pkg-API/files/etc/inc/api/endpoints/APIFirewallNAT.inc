<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallNAT extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/nat";
    }

    protected function get() {
        return (new APIFirewallNATRead())->call();
    }
}