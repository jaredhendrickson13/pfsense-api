<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDpingerRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dpinger/restart";
    }

    protected function post() {
        return (new APIServicesDpingerRestartCreate())->call();
    }
}