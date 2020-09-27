<?php
require_once("api/framework/APIEndpoint.inc");

class APIServices extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services";
    }

    protected function get() {
        return (new APIServicesRead())->call();
    }
}