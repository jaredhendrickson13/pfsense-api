<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUserAuthServerRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-authservers"];
    }

    public function action() {
        if (!empty($this->config["system"]["authserver"])) {
            $as_array = $this->config["system"]["authserver"];
        } else {
            $as_array = [];
        }
        return APIResponse\get(0, $as_array);
    }
}