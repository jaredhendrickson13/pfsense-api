<?php
require_once("api/framework/APIEndpoint.inc");

class APIStatusLogSystem extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/status/log/system";
    }

    protected function get() {
        return (new APIStatusLogSystemRead())->call();
    }
}