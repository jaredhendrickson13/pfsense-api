<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSyslogdStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/syslogd/start";
    }

    protected function post() {
        return (new APIServicesSyslogdStartCreate())->call();
    }
}