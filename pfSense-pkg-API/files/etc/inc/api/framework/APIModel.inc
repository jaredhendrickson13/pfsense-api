<?php
//   Copyright 2020 Jared Hendrickson
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

require_once("api/framework/APITools.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/framework/APIAuth.inc");

class APIModel {
    public $client;
    public $privileges;
    public $initial_data;
    public $validated_data;
    public $id;
    public $validate_id;
    public $config;
    public $errors;
    public $methods;
    public $change_note;
    public $requires_auth;
    public $set_auth_mode;
    public $bypass_read_mode;

    public function __construct() {
        global $config;
        error_reporting(E_ERROR);
        $this->config =& $config;
        $this->privileges = ["page-all"];
        $this->requires_auth = true;
        $this->change_note = "Made unknown change via API";
        $this->validate_id = true;
        $this->initial_data = APITools\get_request_data();
        $this->validated_data = [];
        $this->errors = [];

    }

    public function action() {
        # This function is intended to be overridden by an API model extended class
        # Any configuration writes, system configurations, etc should be added when overriding this base class
        # If this class is not overridden a 500 unexpected error is returned
        return APIResponse\get(10);
    }

    protected function write_config() {
        # Start a temporary session to write the config that contains the user's username in the config history
        session_start();
        $_SESSION["Username"] = $this->client->username;    // This is what adds the username to the change log
        write_config(sprintf(gettext(" ".$this->change_note)));
        unset($_SESSION);
    }

    public function validate_payload() {
        # This function is intended to be overridden by an API model extended class
        # Any payload validation must be specified here
        # If this class is not overridden no validation takes place
    }

    public function validate() {
        $this->check_enable();
        $this->check_server_ip();
        $this->check_version();
        if ($this->requires_auth) {
            $this->check_authentication();
            $this->check_authorization();
        }
        $this->validate_payload();

        if (count($this->errors) === 0) {
            return true;
        } else {
            return false;
        }
    }

    public function call() {
        if ($this->validate()) {
            return $this->action();
        } else {
            return $this->errors[0];
        }
    }

    private function check_authentication() {
        $this->client = new APIAuth($this->privileges, $this->set_auth_mode, $this->bypass_read_mode);
        if ($this->requires_auth === true) {
            if (!$this->client->is_authenticated) {
                $this->errors[] = APIResponse\get(3);
            }
        }
    }

    private function check_authorization() {
        if (!$this->client->is_authorized) {
            $this->errors[] = APIResponse\get(4);
        }
    }

    # Check if the API is enabled before answering calls, if not, redirect to wc login
    private function check_enable() {
        $api_config = APITools\get_api_config()[1];
        if (!isset($api_config["enable"])) {
            header("Location: /");
            die();
        }
    }

    # Check if server is running a supported version of pfSense
    private function check_version() {
        # Local variables
        $curr_ver = str_replace(".", "", explode("-", APITools\get_pfsense_version()["version"])[0]);
        $min_ver = 244;
        $curr_ver = is_numeric($curr_ver) ? intval($curr_ver) : 0;
        if ($curr_ver < $min_ver) {
            $this->errors[] = APIResponse\get(5);
        }
    }

    # Check if server IP is allowed to answer API calls. Redirects to login if not
    private function check_server_ip() {
        $pkg_conf = APITools\get_api_config()[1];
        $allow_ifs = $pkg_conf["allowed_interfaces"];
        $whitelist = explode(",", $allow_ifs);

        // Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            // Check if our server IP is a valid if address, localhost, or any
            if ($_SERVER["SERVER_ADDR"] === $if_info["ipaddr"]) {
                return;
            } elseif ($_SERVER["SERVER_ADDR"] === $if_info["ipaddrv6"]) {
                return;
            } elseif (in_array($_SERVER["SERVER_ADDR"], ["::1", "127.0.0.1", "localhost"]) and $wif === "localhost") {
                return;
            }elseif ($wif === "any") {
                return;
            }
        }

        # Return 444 response if we did not find a previous match
        $this->errors[] = APIResponse\get(6);
    }

}