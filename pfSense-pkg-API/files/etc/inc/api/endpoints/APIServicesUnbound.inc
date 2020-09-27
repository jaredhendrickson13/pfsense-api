<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnbound extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound";
    }

    protected function get() {
        return (new APIServicesUnboundRead())->call();
    }
}