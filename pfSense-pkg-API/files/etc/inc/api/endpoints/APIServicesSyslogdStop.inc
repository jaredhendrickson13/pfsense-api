<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSyslogdStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/syslogd/stop";
    }

    protected function post() {
        return (new APIServicesSyslogdStopCreate())->call();
    }
}