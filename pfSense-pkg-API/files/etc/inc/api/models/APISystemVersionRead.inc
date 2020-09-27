<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemVersionRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-dashboard-widgets", "page-diagnostics-command"];
    }

    public function action() {
        return APIResponse\get(0, APITools\get_pfsense_version());
    }
}