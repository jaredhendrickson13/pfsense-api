<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesNTPdStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/ntpd/start";
    }

    protected function post() {
        return (new APIServicesNTPdStartCreate())->call();
    }
}