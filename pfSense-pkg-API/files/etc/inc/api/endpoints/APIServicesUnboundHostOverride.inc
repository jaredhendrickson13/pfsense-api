<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesUnboundHostOverride extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/unbound/host_override";
    }

    protected function get() {
        return (new APIServicesUnboundHostOverrideRead())->call();
    }
    protected function post() {
        return (new APIServicesUnboundHostOverrideCreate())->call();
    }

    protected function put() {
        return (new APIServicesUnboundHostOverrideUpdate())->call();
    }

    protected function delete() {
        return (new APIServicesUnboundHostOverrideDelete())->call();
    }
}