<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesSSHdStop extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        service_control_stop("sshd", []);
        return APIResponse\get(0, ["sshd" => "stopped"]);
    }
}