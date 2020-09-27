<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSyslogdRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/syslogd/restart";
    }

    protected function post() {
        return (new APIServicesSyslogdRestartCreate())->call();
    }
}