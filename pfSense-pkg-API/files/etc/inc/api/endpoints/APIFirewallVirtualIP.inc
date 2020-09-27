<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallVirtualIP extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/virtual_ip";
    }

    protected function get() {
        return (new APIFirewallVirtualIPRead())->call();
    }

    protected function post() {
        return (new APIFirewallVirtualIPCreate())->call();
    }

    protected function put() {
        return (new APIFirewallVirtualIPUpdate())->call();
    }

    protected function delete() {
        return (new APIFirewallVirtualIPDelete())->call();
    }

}