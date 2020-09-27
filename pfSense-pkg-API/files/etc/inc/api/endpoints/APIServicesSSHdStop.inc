<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSSHdStop extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/sshd/stop";
    }

    protected function post() {
        return (new APIServicesSSHdStopCreate())->call();
    }
}