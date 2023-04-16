<?php
//   Copyright 2023 Jared Hendrickson
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

class APIServicesUnboundApplyCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-services-dnsresolver", "page-services-dnsresolver-edithost"];
    }

    public function action() {
        # Check if the client requested asynchronous application.
        if ($this->initial_data["async"] === true) {
            $this->apply_async();
            return APIResponse\get(14);
        }
        # Otherwise, apply Unbound and wait for it to reload.
        else {
            # Check if application was successful
            if ($this->apply() === true) {
                return APIResponse\get(0);
            }
            else {
                return APIResponse\get(1);
            }
        }
    }

    # TODO: Move unbound_reload_config to this method instead.
    public static function apply() {
        return APITools\unbound_reload_config();
    }

    # Apply Unbound asynchronously
    public static function apply_async() {
        # Execute the helper script asynchronously.
        shell_exec("nohup php -f /usr/local/share/pfSense-pkg-API/scripts/apply_unbound.php > /dev/null &");
    }
}
