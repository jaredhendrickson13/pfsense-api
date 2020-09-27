<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSSHdStart extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/sshd/start";
    }

    protected function post() {
        return (new APIServicesSSHdStartCreate())->call();
    }
}