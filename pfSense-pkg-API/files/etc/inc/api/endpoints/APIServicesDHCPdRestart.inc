<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDHCPdRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dhcpd/restart";
    }

    protected function post() {
        return (new APIServicesDHCPdRestartCreate())->call();
    }
}