<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesNTPdStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/ntpd/stop";
    }

    protected function post() {
        return (new APIServicesNTPdStopCreate())->call();
    }
}