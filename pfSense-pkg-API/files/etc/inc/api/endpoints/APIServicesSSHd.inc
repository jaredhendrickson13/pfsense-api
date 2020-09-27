<?php
require_once("api/framework/APIEndpoint.inc");

class APIServicesSSHd extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/services/sshd";
    }

    protected function get() {
        return (new APIServicesSSHdRead())->call();
    }

    protected function put() {
        return (new APIServicesSSHdUpdate())->call();
    }
}