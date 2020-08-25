<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIServices extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-status-services"];
    }

    public function action() {
        $service_list = get_services();    // Stop our service
        // Loop through our service list and add our service status
        foreach ($service_list as $key => $srvc) {
            $s_status = get_service_status($srvc);
            // Check if service is started
            if ($s_status === true) {
                $service_list[$key]["status"] = "running";
            } elseif ($s_status === false) {
                $service_list[$key]["status"] = "stopped";
            } else {
                $service_list[$key]["status"] = "unknown";
            }
            // Check if user requested only one service, if so remove unmatched services
            if (isset($this->validated_data["name"]) and $this->validated_data["name"] !== $srvc["name"]) {
                unset($service_list[$key]);
            }
        }
        return APIResponse\get(0, $service_list);
    }

    public function validate_payload() {
        if (isset($this->initial_data['name'])) {
            $this->validated_data["name"] = $this->initial_data['name'];
        }
    }
}