<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/start";
    }

    protected function post() {
        return (new APIServicesUnboundStartCreate())->call();
    }
}