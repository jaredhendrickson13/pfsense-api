<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/restart";
    }

    protected function post() {
        return (new APIServicesRestartCreate())->call();
    }
}