<?php
require_once("api/framework/APIEndpoint.inc");

class APIInterfaceVLAN extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/interface/vlan";
    }

    protected function get() {
        return (new APIInterfaceVLANRead())->call();
    }

    protected function post() {
        return (new APIInterfaceVLANCreate())->call();
    }

    protected function put() {
        return (new APIInterfaceVLANUpdate())->call();
    }

    protected function delete() {
        return (new APIInterfaceVLANDelete())->call();
    }

}