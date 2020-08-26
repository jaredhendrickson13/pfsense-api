<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesUnboundApply extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Added DNS Resolver host override via API";
    }

    public function action() {
        // Check if application was successful
        if (APITools\unbound_reload_config() === true) {
            return APIResponse\get(0);
        } else {
            return APIResponse\get(1);
        }
    }
}