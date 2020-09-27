<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIServicesUnboundHostOverrideRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver"];
    }

    public function action() {
        $unbound_array = [];
        if (!empty($this->config["unbound"]["hosts"])) {
            $unbound_array = $this->config["unbound"]["hosts"];
        }
        return APIResponse\get(0, $unbound_array);
    }
}
