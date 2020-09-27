<?php
require_once("api/framework/APIEndpoint.inc");

class APIUserAuthServer extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user/auth_server";
    }

    protected function get() {
        return (new APIUserAuthServerRead())->call();
    }

    protected function delete() {
        return (new APIUserAuthServerDelete())->call();
    }
}