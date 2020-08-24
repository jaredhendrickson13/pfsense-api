<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIUsers extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-system-usermanager"];
    }

    public function action() {
        return APIResponse\get(0, $this->config["system"]["user"]);
    }
}