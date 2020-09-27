<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/stop";
    }

    protected function post() {
        return (new APIServicesUnboundStopCreate())->call();
    }
}