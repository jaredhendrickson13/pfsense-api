<?php
require_once("api/framework/APIEndpoint.inc");

class APIUserAuthServerRADIUS extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user/auth_server/radius";
    }

    protected function get() {
        return (new APIUserAuthServerRADIUSRead())->call();
    }

    protected function delete() {
        return (new APIUserAuthServerRADIUSDelete())->call();
    }
}