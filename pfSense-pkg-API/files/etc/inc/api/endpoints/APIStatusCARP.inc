<?php
require_once("api/framework/APIEndpoint.inc");

class APIStatusCARP extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/status/carp";
    }

    protected function get() {
        return (new APIStatusCARPRead())->call();
    }

    protected function put() {
        return (new APIStatusCARPUpdate())->call();
    }
}