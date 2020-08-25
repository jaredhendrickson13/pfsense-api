<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIServicesUnbound extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-services-dnsresolver"];
    }

    public function action() {
        $unbound_array = [];
        if (!empty($this->config["unbound"])) {
            $unbound_array = $this->config["unbound"];
        }
        return APIResponse\get(0, $unbound_array);
    }
}
