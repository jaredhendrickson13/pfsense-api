<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemAPIRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
    }

    public function action() {
        return APIResponse\get(0, APITools\get_api_config()[1]);
    }
}