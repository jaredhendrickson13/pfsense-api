<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallRule extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/rule";
    }

    protected function get() {
        return (new APIFirewallRuleRead())->call();
    }

    protected function post() {
        return (new APIFirewallRuleCreate())->call();
    }

    protected function put() {
        return (new APIFirewallRuleUpdate())->call();
    }

    protected function delete() {
        return (new APIFirewallRuleDelete())->call();
    }

}