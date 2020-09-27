<?php
require_once("api/framework/APIEndpoint.inc");

class APIFirewallAliasEntry extends APIEndpoint {
    public function __construct() {
        $this->url = "/api/v1/firewall/alias/entry";
    }

    protected function post() {
        return (new APIFirewallAliasEntryCreate())->call();
    }

    protected function delete() {
        return (new APIFirewallAliasEntryDelete())->call();
    }

}