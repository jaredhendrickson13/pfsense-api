<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemTunableRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-advanced-sysctl"];
    }

    public function action() {
        # Convert assoc array to indexed array
        $tunables = [];
        foreach (system_get_sysctls() as $key=>$value) {
            $tunables[] = $value;
        }
        return APIResponse\get(0, $tunables);
    }
}