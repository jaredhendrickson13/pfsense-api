<?php
require_once("api/framework/APIEndpoint.inc");

class APIStatusLogDHCP extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/status/log/dhcp";
    }

    protected function get() {
        return (new APIStatusLogDHCPRead())->call();
    }
}