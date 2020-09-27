<?php
require_once("api/framework/APIEndpoint.inc");

class APIUserGroup extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user/group";
    }

    protected function post() {
        return (new APIUserGroupCreate())->call();
    }

    protected function delete() {
        return (new APIUserGroupDelete())->call();
    }
}