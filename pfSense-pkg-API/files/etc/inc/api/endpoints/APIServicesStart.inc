<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/start";
    }

    protected function post() {
        return (new APIServicesStartCreate())->call();
    }
}