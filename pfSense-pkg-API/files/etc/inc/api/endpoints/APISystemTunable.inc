<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemTunable extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/tunable";
    }

    protected function get() {
        return (new APISystemTunableRead())->call();
    }

    protected function post() {
        return (new APISystemTunableCreate())->call();
    }

    protected function put() {
        return (new APISystemTunableUpdate())->call();
    }

    protected function delete() {
        return (new APISystemTunableDelete())->call();
    }
}