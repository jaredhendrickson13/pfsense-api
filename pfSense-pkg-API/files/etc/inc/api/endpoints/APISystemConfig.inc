<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemConfig extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/config";
    }

    protected function get() {
        return (new APISystemConfigRead())->call();
    }
}