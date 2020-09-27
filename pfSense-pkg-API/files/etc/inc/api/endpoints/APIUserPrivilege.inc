<?php
require_once("api/framework/APIEndpoint.inc");

class APIUserPrivilege extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user/privilege";
    }

    protected function post() {
        return (new APIUserPrivilegeCreate())->call();
    }

    protected function delete() {
        return (new APIUserPrivilegeDelete())->call();
    }
}