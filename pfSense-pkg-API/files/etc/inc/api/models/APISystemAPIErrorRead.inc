<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemAPIErrorRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->requires_auth = false;
    }

    public function action() {
        return APIResponse\get(0, APIResponse\get(0, null, true));
    }
}