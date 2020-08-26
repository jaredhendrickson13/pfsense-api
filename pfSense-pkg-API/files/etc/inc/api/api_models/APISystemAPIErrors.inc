<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemAPIErrors extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->requires_auth = false;
    }

    public function action() {
        return APIResponse\get(0, APIResponse\get(0, null, true));
    }
}