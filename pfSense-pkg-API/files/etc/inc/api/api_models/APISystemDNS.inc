<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemDNS extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->privileges = ["page-all", "page-system"];
    }

    public function action() {
        $this->validate_payload();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {

        $this->validated_data["dnsserver"] = [];
        $this->validated_data["dnsallowoverride"] = false;
        $this->validated_data["dnslocalhost"] = false;
        if (!empty($this->config["system"]["dnsserver"])) {
            $this->validated_data["dnsserver"] = $this->config["system"]["dnsserver"];
        }
        if (array_key_exists("dnsallowoverride", $this->config["system"])) {
            $this->validated_data["dnsallowoverride"] = true;
        }
        if (array_key_exists("dnslocalhost", $this->config["system"])) {
            $this->validated_data["dnslocalhost"] = true;
        }
    }
}
