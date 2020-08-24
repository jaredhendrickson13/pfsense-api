<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemHostname extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-system"];
    }

    public function action() {
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {

        $this->validated_data = ["hostname" => "", "domain" => ""];
        if (!empty($this->config["system"]["hostname"])) {
            $this->validated_data["hostname"] = $this->config["system"]["hostname"];
        }
        // Check that we have a domain configuration
        if (!empty($this->config["system"]["domain"])) {
            $this->validated_data["domain"] = $this->config["system"]["domain"];
        }
    }
}
