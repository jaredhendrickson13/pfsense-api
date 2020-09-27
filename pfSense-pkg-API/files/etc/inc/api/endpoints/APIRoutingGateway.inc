<?php
require_once("api/framework/APIEndpoint.inc");

class APIRoutingGateway extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/routing/gateway";
    }

    protected function get() {
        return (new APIRoutingGatewayRead())->call();
    }
}