<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemARP extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/arp";
    }

    protected function get() {
        return (new APISystemARPRead())->call();
    }

    protected function delete() {
        return (new APISystemARPDelete())->call();
    }
}