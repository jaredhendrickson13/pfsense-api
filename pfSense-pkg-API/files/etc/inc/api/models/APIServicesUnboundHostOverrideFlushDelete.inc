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

class APIServicesUnboundHostOverrideFlushDelete extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver-edithost"];
        $this->change_note = "Flushed DNS Resolver host overrides via API";
        $this->init_config("unbound", "hosts");
    }

    public function action() {
        # Capture the host overrides being deleted, if empty, default to empty array
        $this->validated_data = ($this->config["unbound"]["hosts"]) ?: [];

        # Remove the host overrides from the configuration
        unset($this->config["unbound"]["hosts"]);
        $this->write_config();

        # Mark the Unbound subsystem as changed, but do not allow clients to immediately apply the change.
        mark_subsystem_dirty("unbound");

        return APIResponse\get(0, $this->validated_data);
    }
}
