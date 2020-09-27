<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesSSHdRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-admin"];
    }

    public function action() {
        $ssh_array = [];
        if (!empty($this->config["system"]["ssh"])) {
            $ssh_array = $this->config["system"]["ssh"];
        }
        return APIResponse\get(0, $ssh_array);
    }
}