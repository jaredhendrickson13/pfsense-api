<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemHostname extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/hostname";
    }

    protected function get() {
        return (new APISystemHostnameRead())->call();
    }

    protected function put() {
        return (new APISystemHostnameUpdate())->call();
    }
}