<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundHostOverrideAlias extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/host_override/alias";
    }

    protected function post() {
        return (new APIServicesUnboundHostOverrideAliasCreate())->call();
    }
}