<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemDNSServer extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/dns/server";
    }

    protected function post() {
        return (new APISystemDNSServerCreate())->call();
    }

    protected function delete() {
        return (new APISystemDNSServerDelete())->call();
    }
}