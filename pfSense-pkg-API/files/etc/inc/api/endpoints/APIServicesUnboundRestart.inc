<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/restart";
    }

    protected function post() {
        return (new APIServicesUnboundRestartCreate())->call();
    }
}