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

namespace API\Core;

require_once("api/core/Tools.inc");
require_once("api/core/Response.inc");
require_once("api/core/Auth.inc");

use API;

class Model {
    public $packages;
    public $package_includes;
    public $initial_data;
    public $validated_data;
    public $id;
    public $validate_id;
    public $errors;
    public $change_note;
    public $client;

    public function __construct($initial_data=[], $client=null) {
        $this->packages = [];
        $this->package_includes = [];
        $this->change_note = "Made unknown change via API";
        $this->validate_id = true;
        $this->initial_data = $initial_data;
        $this->validated_data = [];
        $this->errors = [];

        # Allow a client API\Core\Auth object to be assigned to this model
        if ($client) {
            $this->client = $client;
        }
        # Otherwise, define the default system client if one was not specified
        else {
            $this->client = new API\Core\Auth();
            $this->client->username = "(system api)";
            $this->client->ip_address = "127.0.0.1";
        }
    }

    public function action() {
        # This function is intended to be overridden by an API model extended class
        # Any configuration writes, system configurations, etc should be added when overriding this base class
        # If this class is not overridden a 500 unexpected error is returned
        return API\Core\Response\get(10);
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
        $conf = Model::get_config($path);

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
            throw new \Error("Next array ID was requested for existing non-array value");
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
        # Check for required packages
        $this->check_packages();

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
            # Bypass the action if requested
            if ($this->initial_data["_action_bypass"] === true) {
                return API\Core\Response\get(15);
            } else {
                return $this->action();
            }
        } else {
            return $this->errors[0];
        }
    }

    # Check if all required packages are installed. Sets an error otherwise.
    private function check_packages() {
        # Loop through each required package and ensure it is present on the system.
        foreach ($this->packages as $pkg) {
            # Return an error if the package is not installed
            if (!is_pkg_installed($pkg)) {
                $this->errors[] = API\Core\Response\get(12);
                return false;
            }
        }

        # Loop through each package dependency and add it.
        foreach ($this->package_includes as $include) {
            require_once($include);
        }

        return true;
    }

    /**
     * Initializes the default '_read' method. Child classes will override this class to add functionality that
     * performs a 'read' action for this object (if applicable). If not overridden, an API error will simply return
     * a method not implemented error. This method is not intended to be called directly, rather it should be called
     * using the public 'read' method.
     * @return array an associative array containing the API response
     */
    public function _read() : array {
        return API\Core\Response\get(2);
    }

    /**
     * Execute the read action of the current model. In addition to calling the object's
     * _read() method, this method will allow for an optional query to be performed as well as optionally
     * embedding the object ID in the API response.
     * @return array an associative array containing the API response
     */
    public function read() : array {
        return $this->_read();
    }

    /**
     * Initializes the default 'create' method. Child classes will override this class to add functionality that
     * performs a 'create' action for this object (if applicable). If not overridden, an API error will simply return
     * a method not implemented error. This method is not intended to be called directly, rather it should be called
     * using the public 'create' method.
     * @return array an associative array containing the API response
     */
    public function _create() : array {
        return API\Core\Response\get(2);
    }

    /**
     * Perform validation and execute the create action of the current model.
     * @return array an associative array containing the API response
     */
    public function create() : array {
        # If the API call was valid, execute the action. Otherwise, return the first error encountered.
        if ($this->validate()) {
            # Bypass the action if requested
            if ($this->initial_data["_action_bypass"] === true) {
                return API\Core\Response\get(15);
            } else {
                return $this->_create();
            }
        } else {
            return $this->errors[0];
        }
    }

    /**
     * Initializes the default 'update' method. Child classes will override this class to add functionality that
     * performs an 'update' action for this object (if applicable). If not overridden, an API error will simply return
     * a method not implemented error. This method is not intended to be called directly, rather it should be called
     * using the public 'update' method.
     * @return array an associative array containing the API response
     */
    public function _update() : array {
        return API\Core\Response\get(2);
    }

    /**
     * Perform validation and execute the update action of the current model.
     * @return array an associative array containing the API response
     */
    public function update() : array {
        # If the API call was valid, execute the action. Otherwise, return the first error encountered.
        if ($this->validate()) {
            # Bypass the action if requested
            if ($this->initial_data["_action_bypass"] === true) {
                return API\Core\Response\get(15);
            } else {
                return $this->_update();
            }
        } else {
            return $this->errors[0];
        }
    }

    /**
     * Initializes the default 'delete' method. Child classes will override this class to add functionality that
     * performs an 'delete' action for this object (if applicable). If not overridden, an API error will simply return
     * a method not implemented error. This method is not intended to be called directly, rather it should be called
     * using the public 'delete' method.
     * @return array an associative array containing the API response
     */
    public function _delete() : array {
        return API\Core\Response\get(2);
    }

    /**
     * Perform validation and execute the delete action of the current model.
     * @return array an associative array containing the API response
     */
    public function delete() : array {
        # If the API call was valid, execute the action. Otherwise, return the first error encountered.
        if ($this->validate()) {
            # Bypass the action if requested
            if ($this->initial_data["_action_bypass"] === true) {
                return API\Core\Response\get(15);
            } else {
                return $this->_delete();
            }
        } else {
            return $this->errors[0];
        }
    }
}
