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

require_once("api/framework/APITools.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/framework/APIAuth.inc");

class APIModel {
    public $client;
    public $privileges;
    public $packages;
    public $package_includes;
    public $initial_data;
    public $validated_data;
    public $type_map;
    public $id;
    public $validate_id;
    public $config;
    public $errors;
    public $methods;
    public $change_note;
    public $requires_auth;
    public $set_auth_mode;
    public $retain_read_mode;
    public $ignore_ifs;
    public $ignore_enabled;

    public function __construct() {
        global $config;
        error_reporting(E_ERROR);    // Prevents PHP warnings from printing to API responses
        $this->config =& $config;
        $this->privileges = ["page-all"];
        $this->packages = [];
        $this->package_includes = [];
        $this->requires_auth = true;
        $this->change_note = "Made unknown change via API";
        $this->validate_id = true;
        $this->initial_data = APITools\get_request_data();
        $this->validated_data = [];
        $this->errors = [];
        $this->type_map = [];
        $this->retain_read_mode = true;
        $this->ignore_ifs = false;
        $this->ignore_enabled = false;
    }

    public function action() {
        # This function is intended to be overridden by an API model extended class
        # Any configuration writes, system configurations, etc should be added when overriding this base class
        # If this class is not overridden a 500 unexpected error is returned
        return APIResponse\get(10);
    }

    /**
     * Initialize the configuration array of a specific config path
     * @param $path string config path with '/' as separators
     * @returns null
     */
    public static function init_config(string $path) {
        # Initialize the configuration array of a specified path.
        global $config;
        array_init_path($config, $path);
    }

    /**
     * Read the value of specific configuration section by path.
     * @param $path string config path with '/' as separators
     * @param $default mixed value to return if the path is not found
     * @returns mixed value at path or $default if the path does not exist or if the
     *          path keys an empty string and $default is non-null
     */
    public static function get_config(string $path, $default=null) {
        global $config;
        return(array_get_path($config, $path, $default));
    }

    /**
     * Set a value by path in the config.
     * @param $path string path with '/' separators
     * @param $value mixed value to set
     * @param $default mixed value to return if the path is not found
     * @returns mixed $val or $default if the path prefix does not exist
     */
    public static function set_config(string $path, $value, $default=null) {
        global $config;
        return (array_set_path($config, $path, $value, $default));
    }

    /**
     * Delete a key from the configuration
     * @param $path string path with '/' separators
     * @returns array copy of the removed value or null
     */
    public static function del_config(string $path) {
        global $config;
        return (array_del_path($config, $path));
    }

    /**
     * Checks if a specific config value is set. Toggle values are often set when
     * enabled, or missing when they are not. This check is better suited for
     * pfSense configurations than isset().
     * @param $path string path with '/' separators
     * @param $enable_key string an optional alternative key value for the enable key
     * @returns bool true if $enable_key exists in the array at $path, and has a
     * non-null value, otherwise false.
     */
    public static function is_config_enabled(string $path, string $enable_key="enable") {
        global $config;
        return (array_path_enabled($config, $path, $enable_key));
    }

    /**
     * Finds the next available ID for a specific configuration array. This function
     * only works for numeric-keyed arrays in the configuration.
     * @param $path string path with '/' separators
     * @throws Error when the path leads to a existing non-empty, non-array value
     * @returns int the next array key (ID) for the config path specified
     */
    public static function get_next_id(string $path) {
        # First, get the configuration at the specific path
        $conf = APIModel::get_config($path);

        # Ensure config is array
        if (is_array($conf)) {
            return count($conf);
        }
        # If the value is empty, start the array at ID 0
        elseif (empty($conf)) {
            return 0;
        }
        # Otherwise, this action could be dangerous, throw an error
        else {
            throw new Error("Next array ID was requested for existing non-array value");
        }
    }
    /**
     * Write configuration changes to the config file
     */
    public function write_config() {
        # Local variables
        $lock_file = "/tmp/.api.write_config.lock";

        # Ensure there is not an API config lock, loop until the lock is released
        foreach (range(0, 60) as $attempt) {
            # Only write to the config if there is not an active lock
            if (!file_exists($lock_file)) {
                # Create a lock on API config writes while we write changes
                file_put_contents($lock_file, microtime());

                # Start a temporary session to write the config that contains the user's username in the config history
                session_start();
                $_SESSION["Username"] = $this->client->username;
                write_config(sprintf(gettext(" ".$this->change_note)));
                unset($_SESSION);
                phpsession_destroy();
                unlink($lock_file);
                return;
            }

            # Delay attempts by 1 second
            sleep(1);
        }
    }

    public function validate_payload() {
        # This function is intended to be overridden by an API model extended class
        # Any payload validation must be specified here
        # If this class is not overridden no validation takes place
    }

    public function validate() {
        # Checks API status and requirements
        $this->check_enable();
        $this->check_server_ip();
        $this->check_packages();

        # Checks request data
        $this->check_request_data();

        # Checks authentication and authorization if required
        if ($this->requires_auth) {
            $this->check_authentication();
            $this->check_authorization();
        }

        # Attempt to validate the request
        $this->validate_payload();

        # All checks pass if no errors were encountered. Otherwise, validation failed.
        if (count($this->errors) === 0) {
            return true;
        } else {
            return false;
        }
    }

    public function call() {
        # If the API call was valid, execute the action. Otherwise, return the first error encountered.
        if ($this->validate()) {
            return $this->action();
        } else {
            return $this->errors[0];
        }
    }

    private function check_authentication() {
        $read_only = (isset(APITools\get_api_config()[1]["readonly"]) and $this->retain_read_mode) ? true : false;
        $this->client = new APIAuth($this->privileges, $this->set_auth_mode, $read_only);
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
        if (!isset($api_config["enable"]) and !$this->ignore_enabled) {
            header("Location: /");
            die();
        }
    }

    # Check if all required packages are installed. Sets an error otherwise.
    private function check_packages() {
        # Loop through each required package and ensure it is present on the system.
        foreach ($this->packages as $pkg) {
            # Return an error if the package is not installed
            if (!is_pkg_installed($pkg)) {
                $this->errors[] = APIResponse\get(12);
                return false;
            }
        }

        # Loop through each package dependency and add it.
        foreach ($this->package_includes as $include) {
            require_once($include);
        }

        return true;
    }

    # Check if server IP is allowed to answer API calls. Redirects to login if not
    private function check_server_ip() {
        $pkg_conf = APITools\get_api_config()[1];
        $allow_ifs = $pkg_conf["allowed_interfaces"];
        $whitelist = explode(",", $allow_ifs);

        # Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            # Check if our server IP is a valid if address, VIP, localhost, or any
            if ($_SERVER["SERVER_ADDR"] === $if_info["ipaddr"] or APITools\is_ip_vip($_SERVER["SERVER_ADDR"], $wif)) {
                return;
            } elseif ($_SERVER["SERVER_ADDR"] === $if_info["ipaddrv6"]) {
                return;
            } elseif (in_array($_SERVER["SERVER_ADDR"], ["::1", "127.0.0.1", "localhost"]) and $wif === "localhost") {
                return;
            } elseif ($wif === "any") {
                return;
            } elseif ($this->ignore_ifs === true) {
                return;
            }
        }

        # Return 444 response if we did not find a previous match
        $this->errors[] = APIResponse\get(6);
    }

    # Check if our requested content-type is supported and parsed data correctly, sets error if not
    private function check_request_data() {
        if ($this->initial_data === false) {
            $this->errors[] = APIResponse\get(11);
        }
    }
}
