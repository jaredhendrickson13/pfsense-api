<?php
require_once("api/framework/APIEndpoint.inc");

class APIInterface extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/interface";
    }

    protected function get() {
        return (new APIInterfaceRead())->call();
    }

    protected function post() {
        return (new APIInterfaceCreate())->call();
    }

    protected function delete() {
        return (new APIInterfaceDelete())->call();
    }

}