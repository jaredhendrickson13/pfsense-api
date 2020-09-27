<?php
require_once("api/framework/APIEndpoint.inc");

class APIUser extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/user";
    }

    protected function get() {
        return (new APIUserRead())->call();
    }

    protected function post() {
        return (new APIUserCreate())->call();
    }

    protected function put() {
        return (new APIUserUpdate())->call();
    }

    protected function delete() {
        return (new APIUserDelete())->call();
    }
}