<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemCertificateRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-certmanager"];
    }

    public function action() {
        $this->validate_payload();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (!empty($this->config["cert"])) {
            $this->validated_data["cert"] = $this->config["cert"];
        }
    }
}
