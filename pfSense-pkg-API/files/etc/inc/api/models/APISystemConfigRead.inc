<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemConfigRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-diagnostics-backup-restore", "page-diagnostics-command"];
    }

    public function action() {
        return APIResponse\get(0, $this->config);
    }
}