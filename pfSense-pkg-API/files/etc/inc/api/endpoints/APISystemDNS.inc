<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemDNS extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/dns";
    }

    protected function get() {
        return (new APISystemDNSRead())->call();
    }

    protected function put() {
        return (new APISystemDNSUpdate())->call();
    }
}