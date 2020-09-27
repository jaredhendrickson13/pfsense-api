<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundApply extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/apply";
    }

    protected function post() {
        return (new APIServicesUnboundApplyCreate())->call();
    }
}