<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesNTPdRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/ntpd/restart";
    }

    protected function post() {
        return (new APIServicesNTPdRestartCreate())->call();
    }
}