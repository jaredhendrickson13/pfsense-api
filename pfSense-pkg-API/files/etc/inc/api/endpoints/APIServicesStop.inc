<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/stop";
    }

    protected function post() {
        return (new APIServicesStopCreate())->call();
    }
}