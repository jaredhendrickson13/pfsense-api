<?php
require_once("api/framework/APIEndpoint.inc");

class APISystemTable extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/system/table";
    }

    protected function get() {
        return (new APISystemTableRead())->call();
    }
}