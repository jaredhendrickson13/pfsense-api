<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServicesStartCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        $services = get_services();
        // Loop through our service list and add our service status
        foreach ($services as $key => $srvc) {
            service_control_start($srvc["name"], []);    // Start our service
            $services[$key]["status"] = "started";
        }
        return APIResponse\get(0, $services);
    }
}