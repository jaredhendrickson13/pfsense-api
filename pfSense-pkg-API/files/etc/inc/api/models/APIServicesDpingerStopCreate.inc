<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesDpingerStopCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        service_control_stop("dpinger", []);
        return APIResponse\get(0, ["dpinger" => "stopped"]);
    }
}