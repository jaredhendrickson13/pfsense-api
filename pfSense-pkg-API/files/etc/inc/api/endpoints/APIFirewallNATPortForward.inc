<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallNATPortForward extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/nat/port_forward";
    }

    protected function get() {
        return (new APIFirewallNATPortForwardRead())->call();
    }

    protected function post() {
        return (new APIFirewallNATPortForwardCreate())->call();
    }

    protected function delete() {
        return (new APIFirewallNATPortForwardDelete())->call();
    }

}