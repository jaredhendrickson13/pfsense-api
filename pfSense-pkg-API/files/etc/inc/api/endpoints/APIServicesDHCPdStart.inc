<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDHCPdStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dhcpd/start";
    }

    protected function post() {
        return (new APIServicesDHCPdStartCreate())->call();
    }
}