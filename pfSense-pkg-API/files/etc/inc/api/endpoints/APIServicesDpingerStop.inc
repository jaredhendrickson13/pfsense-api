<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesDpingerStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/dpinger/stop";
    }

    protected function post() {
        return (new APIServicesDpingerStopCreate())->call();
    }
}