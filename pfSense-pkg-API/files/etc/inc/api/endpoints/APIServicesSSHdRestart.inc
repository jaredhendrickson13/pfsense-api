<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSSHdRestart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/sshd/restart";
    }

    protected function post() {
        return (new APIServicesSSHdRestartCreate())->call();
    }
}