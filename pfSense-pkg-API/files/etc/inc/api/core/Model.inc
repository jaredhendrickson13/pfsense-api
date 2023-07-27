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

# Allow models to access all API validators
foreach(glob("/etc/inc/api/validators/*.inc") as $validator) {
    require_once($validator);
}

# Allow models to access all API fields
foreach(glob("/etc/inc/api/fields/*.inc") as $field) {
    require_once($field);
}

use API;

/**
 * Creates a Model object that relates to a pfSense configuration or service. This model defines the necessary steps
 * to validate incoming configuration and create, read, update, and/or delete data from the pfSense configuration this
 * object relates to.
 */
class Model {
    public string|null $config_path = null;
    public bool $many = false;

    public mixed $id = null;
    public API\Core\Auth $client;
    public array $packages = [];
    public array $package_includes = [];
    public array $initial_data = [];
    public string $change_note = "Made unknown change via API";

    /**
     * Values to assign when a Model object is first constructed.
     * @param array $initial_data The data to use when validating/creating/updating/reading/deleting this object.
     * @param Auth|null $client Optionally assign an API client's `Auth` object. Required for tying users to changes made through Models.
     */
    public function __construct(mixed $id=null, array $initial_data=[], API\Core\Auth $client=null) {
        # Use the explicit $id parameter if set otherwise attempt to obtain it from the $initial_data
        $this->id = (isset($id)) ? $id : $initial_data["id"];
        $this->initial_data = $initial_data;

        # If an ID is present, fetch the object from configuration
        if (isset($this->id)) {
            $this->refresh_from_config($this->id);
        }

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

    /**
     * Initialize the configuration array of a specific config path
     * @param $path string config path with '/' as separators
     */
    protected static function init_config(string $path) {
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
    public static function get_config(string $path, mixed $default=null) {
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
    protected static function set_config(string $path, mixed $value, mixed $default=null) {
        global $config;
        return (array_set_path($config, $path, $value, $default));
    }

    /**
     * Delete a key from the configuration
     * @param $path string path with '/' separators
     * @returns array copy of the removed value or null
     */
    protected static function del_config(string $path) : array {
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
    protected static function is_config_enabled(string $path, string $enable_key="enable") : bool {
        global $config;
        return (array_path_enabled($config, $path, $enable_key));
    }

    /**
     * Finds the next available ID for a specific configuration array. This function
     * only works for numeric-keyed arrays in the configuration.
     * @param $path string path with '/' separators
     * @throws \Error when the path leads to a existing non-empty, non-array value
     * @returns int the next array key (ID) for the config path specified
     */
    protected static function get_next_id(string $path) : int {
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
    protected function write_config() {
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

    /**
     * Obtain this Model object from the internal pfSense configuration by object ID. If the specified ID exists in
     * config, this Model object will be overwritten with the contents of that object.
     * @param mixed $id Specify the ID of the object to obtain.
     * @throws ServerError When this Model does not have a `config_path` set.
     * @throws NotFoundError When an object with the specified $id does not exist.
     */
    public function refresh_from_config(mixed $id ) {
        # Variables
        $internal_objects = $this->get_config($this->config_path, []);

        # Throw an error if no config_path is set
        if (!$this->config_path) {
            throw new ServerError(
                message: "Model must have a 'config_path' to get internal values from config.",
                return: "MODEL_NO_CONFIG_PATH"
            );
        }

        # If an ID was requested that doesn't exist, throw a not found error
        if (!array_key_exists($id, $internal_objects)) {
            throw new NotFoundError(
                message: "Object with ID '$id' does not exist.",
                return: "MODEL_OBJECT_NOT_FOUND"
            );
        }

        # Set this objects ID to the ID requested
        $this->id = $id;

        # Loop through each of this Model's fields and convert the internal value to the representation value
        foreach ($this->get_fields() as $field) {
            # Assign names and model context to this field
            $this->$field->context = $this;
            $this->$field->name = $field;
            $this->$field->check_field_names();

            # Convert the internal value to it's representation value if it exists in the internal object
            if (array_key_exists($this->$field->internal_name, $internal_objects[$id])) {
                $this->$field->from_internal($internal_objects[$id][$this->$field->internal_name]);
            }
        }
    }

    /**
     * Fetches Model objects for all objects stored in the internal pfSense configuration for this Model.
     * @return array An array of Model objects fetched from the internal pfSense configuration.
     */
    private function get_all_from_config() : array {
        # Only allow this method if this model has `many` enabled.
        if (!$this->many) {
            throw new ServerError(
                message: "Cannot get all model objects from config on model without `many`",
                return: "MODEL_WITHOUT_MANY"
            );
        }

        # Variables
        $model = get_class($this);
        $model_objects = [];
        $internal_objects = $this->get_config($this->config_path, []);

        # Loop through each internal object and create a Model object for it
        foreach ($internal_objects as $internal_id=>$internal_object) {
            $model_object = new $model();
            $model_object->refresh_from_config($internal_id);
            $model_objects[] = $model_object;
        }

        return $model_objects;
    }

    /**
     * Creates a serializable array representation of this object. This array is suitable for content
     * serialization such as JSON, YAML, etc.
     * @return array The serializable array representation of this object.
     */
    public function to_representation() : array {
        # Variables
        $representation = [];

        # Embed this object's ID if set
        if (isset($this->id)) {
            $representation["id"] = $this->id;
        }

        # Loop through each of this Model's fields and add its value to a serializable array
        foreach ($this->get_fields() as $field) {
            $representation[$field] = $this->$field->value;
        }

        return $representation;
    }

    public function to_internal() : array {
        # Variables
        $internal_object = [];

        # Loop through each of this Model's fields and add its value to a serializable array
        foreach ($this->get_fields() as $field) {
            $internal_object[$this->$field->internal_name] = $this->$field->to_internal();
        }

        return $internal_object;
    }

    /**
     * Populates an array of field names for this model.
     * @return array An array of field names for this model.
     */
    public function get_fields() : array {
        # Variables
        $fields = [];

        # Loop through each var in this object and extract the objects with a base class of API\Core\Field.
        foreach (get_object_vars($this) as $var => $value) {
            # Skip this var if it's not an object
            if (!is_object($value)) {
                continue;
            }

            # Add this var to our fields if API\Core\Field is this object's ancestor
            if (in_array("API\Core\Field", class_parents($value))) {
                $fields[] = $var;
            }
        }

        return $fields;
    }

    /**
     * Performs a full validation on the API mode. This includes checking that required packages are installed and
     * validating all assigned fields. Note: Fields are validated in the order they are defined in the class.
     * @return bool `true` if the model's data is valid.
     * @throws ServerError When a server or programming error is detected.
     * @throws ValidationError When model validation fails.
     */
    public function validate() : bool {
        # Check for required packages
        $this->check_packages();

        # Obtain the names of Fields assigned to this object
        $fields = $this->get_fields();;

        # Loop through each of this object's assigned fields and validate them.
        foreach ($fields as $field) {
            # Assign the name of this field and context to this Field object
            $this->$field->name = $field;
            $this->$field->context = $this;

            # Run validation on this field
            $this->$field->validate();

            # Check if a custom validation method was implemented for this model
            $custom_validate_method = "validate_$field";
            if (method_exists($this, $custom_validate_method)) {
                # Run the custom validation method and reassign the fields value to its return value.
                $this->$field->value = $this->$custom_validate_method($this->$field->value);
            }
        }

        return true;
    }

    /**
     * Checks if the required pfSense packages for this model are installed.
     * @return bool `true` if all required pfSense packages are installed, `false` if any required pfSense packages
     *      are not installed.
     */
    private function check_packages() : bool {
        # Loop through each required package and ensure it is present on the system.
        foreach ($this->packages as $pkg) {
            # Return an error if the package is not installed
            if (!is_pkg_installed($pkg)) {
                throw new NotFoundError(
                    message: "The requested action requires the '$pkg' package but it is not installed.",
                    return: "MDOEL_MISSING_REQUIRED_PACKAGE"
                );
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
        # Throw an error if a config path was not defined.
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
