<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APIStatusCarp extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-carp"];
        $this->methods = ["GET"];
        $this->validators = [];
    }

    public function action() {
        ;
        $carp_status = [];
        $carp_status["enable"] = APITools\is_carp_enabled();
        $carp_status["maintenance_mode"] = isset($this->config["virtualip_carp_maintenancemode"]);
        $carp_status["interfaces"] = APITools\get_carp_if_status();
        return APIResponse\get(0, $carp_status);
    }
}