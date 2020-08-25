<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIRoutingGateways extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-system-gateways"];
    }

    public function action() {
        $gw_array = [];
        if (!empty($this->config["gateways"]["gateway_item"])) {
            $gw_array = $this->config["gateways"]["gateway_item"];
        }
        return APIResponse\get(0, $gw_array);
    }
}