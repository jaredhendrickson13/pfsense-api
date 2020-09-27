<?php
require_once("api/framework/APIEndpoint.inc");

class APIStatusLogFirewall extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/status/log/firewall";
    }

    protected function get() {
        return (new APIStatusLogFirewallRead())->call();
    }
}