<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemAPI extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/api";
    }

    protected function get() {
        return (new APISystemAPIRead())->call();
    }
}