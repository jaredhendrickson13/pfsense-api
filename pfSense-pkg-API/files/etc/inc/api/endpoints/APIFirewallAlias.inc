<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallAlias extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/alias";
    }

    protected function get() {
        return (new APIFirewallAliasRead())->call();
    }

    protected function post() {
        return (new APIFirewallAliasCreate())->call();
    }

    protected function put() {
        return (new APIFirewallAliasUpdate())->call();
    }

    protected function delete() {
        return (new APIFirewallAliasDelete())->call();
    }

}