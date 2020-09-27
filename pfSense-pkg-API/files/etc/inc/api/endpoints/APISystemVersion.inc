<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemVersion extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/version";
    }

    protected function get() {
        return (new APISystemVersionRead())->call();
    }
}