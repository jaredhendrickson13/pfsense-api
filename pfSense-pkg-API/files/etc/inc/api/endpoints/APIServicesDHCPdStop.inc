<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDHCPdStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dhcpd/stop";
    }

    protected function post() {
        return (new APIServicesDHCPdStopCreate())->call();
    }
}