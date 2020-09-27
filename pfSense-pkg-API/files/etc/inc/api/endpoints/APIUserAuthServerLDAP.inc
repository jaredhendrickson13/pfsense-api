<?php
require_once("api/framework/APIEndpoint.inc");

class APIUserAuthServerLDAP extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user/auth_server/ldap";
    }

    protected function get() {
        return (new APIUserAuthServerLDAPRead())->call();
    }

    protected function post() {
        return (new APIUserAuthServerLDAPCreate())->call();
    }

    protected function delete() {
        return (new APIUserAuthServerLDAPDelete())->call();
    }
}