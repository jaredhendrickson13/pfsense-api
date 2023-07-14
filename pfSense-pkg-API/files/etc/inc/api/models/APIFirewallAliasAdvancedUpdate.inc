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

class APIFirewallAliasAdvancedUpdate extends APIModel {
    public $alias_interval_changed = false;

    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Modified advanced firewall alias settings via API";
    }

    public function action() {
        # Write these changes to the config
        $this->set_config("system", array_replace($this->get_config("system"), $this->validated_data));
        $this->write_config();

        # Restart filterdns when the alias interval changes
        if ($this->alias_interval_changed) {
            $this->restart_filterdns();
        }

        # Reload the filter to apply changes
        filter_configure();

        return APIResponse\get(
            0,
            [
                "aliasesresolveinterval" => intval($this->get_config("system/aliasesresolveinterval")),
                "checkaliasesurlcert" => $this->is_config_enabled("system", "checkaliasesurlcert")
            ]
        );
    }

    public function validate_payload() {
        $this->__validate_aliasesresolveinterval();
        $this->__validate_checkaliasesurlcert();
    }

    private function __validate_aliasesresolveinterval() {
        # Check for the optional 'aliasesresolveinterval' field
        if (isset($this->initial_data["aliasesresolveinterval"])) {
            # Require value to be numeric integer
            if (is_numeric($this->initial_data["aliasesresolveinterval"])) {
                $this->validated_data["aliasesresolveinterval"] = intval($this->initial_data["aliasesresolveinterval"]);
                $this->alias_interval_changed = true;
            }
            else {
                $this->errors[] = APIResponse\get(4242);
            }
        }
    }

    private function __validate_checkaliasesurlcert() {
        # Check for the optional 'checkaliasesurlcert' field
        if ($this->initial_data["checkaliasesurlcert"] === true) {
            $this->validated_data["checkaliasesurlcert"] = true;
        }
        elseif ($this->initial_data["checkaliasesurlcert"] === false) {
            $this->del_config("system/checkaliasesurlcert");
        }
    }

    public static function restart_filterdns() {
        global $g;

        # If the filterdns process ID is present, kill it.
        if (isvalidpid("{$g['varrun_path']}/filterdns.pid")) {
            killbypid("{$g['varrun_path']}/filterdns.pid");
        }
    }


}
