<?php
//   Copyright 2022 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APISystemDNSRead extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system"];
    }

    public function action() {
        $this->validate_payload();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Initialize DNS items
        $this->validated_data["dnsserver"] = [];
        $this->validated_data["dnsallowoverride"] = false;
        $this->validated_data["dnslocalhost"] = false;

        # Populate our system DNS servers if they exist
        if (!empty($this->config["system"]["dnsserver"])) {
            $this->validated_data["dnsserver"] = $this->config["system"]["dnsserver"];
        }

        # Format our DNS allow override on WAN
        if (array_key_exists("dnsallowoverride", $this->config["system"])) {
            $this->validated_data["dnsallowoverride"] = true;
        }

        # Format our DNS local host value
        if ($this->config["system"]["dnslocalhost"] === "remote") {
            $this->validated_data["dnslocalhost"] = false;
        } else {
            $this->validated_data["dnslocalhost"] = true;
        }


    }
}
