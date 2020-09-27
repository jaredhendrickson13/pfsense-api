<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemAPIError extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/api/error";
    }

    protected function get() {
        return (new APISystemAPIErrorRead())->call();
    }
}