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
require_once("api/core/ModelSet.inc");

# Allow models to access all API validators
foreach(glob("/etc/inc/api/validators/*.inc") as $validator) {
    require_once($validator);
}

# Allow models to access all API fields
foreach(glob("/etc/inc/api/fields/*.inc") as $field) {
    require_once($field);
}

use API;
use Exception;
use ReflectionClass;

# Constants
const DEFAULT_CLIENT_USERNAME = "(system api)";
const DEFAULT_CLIENT_IP_ADDRESS = "1.2.3.4";

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
    public array $representation_data = [];
    public string $internal_callable = "";
    public string $subsystem = "";
    public string $verbose_name = "";
    public string $verbose_name_plural = "";

    /**
     * Values to assign when a Model object is first constructed.
     * @param mixed $id If specified, the internal object with this ID will be obtained as the Model object.
     * @param array $representation_data Allows objects to be obtained, created and/or updated using an array of
     * field-value pairs. If this array contains an `id` field, the internal object with this ID will be obtained as
     * the Model object first. Any additional fields will be used to set the Model object's values.
     * @param Auth|null $client Optionally assign an API client's `Auth` object. Required for tying users to
     * changes made through Models.
     */
    public function __construct(mixed $id=null, array $representation_data=[], API\Core\Auth $client=null) {
        # Throw an error if both $id and $representation_data are set.
        if (isset($id) and $representation_data) {
            throw new ServerError(
                message: "Model objects cannot be constructed with both an ID and representation data.",
                ref_id: "MODEL_WITH_ID_AND_REPRESENTATION"
            );
        }

        # Use the ID to obtain this Model object if present.
        if (isset($id)) {
            $this->from_internal($id);
        }

        # Use the representation to obtain or construct this object if present.
        if ($this->representation_data) {
            $this->from_internal($id);
        }

        # Allow an Auth object to be assigned the model. Allows models to view a client's IP address and username.
        if ($client) {
            $this->client = $client;
        }
        # Otherwise, define a default system client if one was not specified.
        else {
            $this->client = new API\Core\Auth();
            $this->client->username = DEFAULT_CLIENT_USERNAME;
            $this->client->ip_address = DEFAULT_CLIENT_IP_ADDRESS;
        }

        # Set the verbose and verbose plural names for this object if not specified
        if (!$this->verbose_name) {
            $model_name = (new ReflectionClass($this))->getShortName();;
            $this->verbose_name = preg_replace('/([a-z])([A-Z])/s','$1 $2', $model_name);
        }
        if (!$this->verbose_name_plural) {
            $plural_suffix = (str_ends_with($this->verbose_name, "s")) ? "es" : "s";
            $this->verbose_name_plural = $this->verbose_name.$plural_suffix;
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
     * Default method for obtaining the next ID from the pfSense configuration. This searches for the next index ID for
     * the model in the pfSense configuration and requires the model's `config_path` and `many` properties to be set.
     * For models that do not use a `config_path` or does not use config index IDs, this method can be overridden by
     * the child model class to add custom processes to obtain the next available ID.
     * @throws ServerError When the path leads to an existing non-empty, non-sequential array value
     * @returns int the next array key (ID) for the config path specified.
     */
    public function get_next_id() : int {
        # Since this references config IDs, this method requires `config_path` to be set. Throw an error if it's not.
        if (!$this->config_path) {
            throw new ServerError(
                message: "Model cannot use default method to obtain next available ID without 'config_path'",
                ref_id: "MODEL_NEXT_ID_WITHOUT_CONFIG_PATH"
            );
        }

        # IDs are only applicable for `many` Models. Throw an error if `many` is not set.
        if (!$this->many) {
            throw new ServerError(
                message: "Models cannot obtain next ID when 'many' is not enabled.",
                ref_id: "MODEL_NEXT_ID_WITHOUT_MANY"
            );
        }

        # First, get the configuration at the specific path
        $conf = $this->get_config($this->config_path);

        # Ensure config is an array
        if (is_array($conf) and array_key_exists(0, $conf)) {
            return count($conf);
        }
        # If the value is empty, start the array at ID 0
        elseif (empty($conf)) {
            return 0;
        }
        # Otherwise, this action could be dangerous, throw an error
        else {
            throw new ServerError(
                message: "Model failed to obtain the next available ID because the config path leads to an existing, 
                non-indexed array value.",
                ref_id: "MODEL_DANGEROUS_CONFIG_PATH"
            );
        }
    }

    /**
     * Write configuration changes to the config file
     * @param string $change_note The message to write to the change log.
     */
    protected function write_config(string $change_note) {
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
                write_config(sprintf(gettext(" ".$change_note)));

                # If a subsystem is specified for this Model, mark it as dirty
                if ($this->subsystem) {
                    mark_subsystem_dirty($this->subsystem);
                }

                # Remove the temporary session and write lock
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
     * Converts this Model object into it's internal pfSense configuration form.
     * @return array The internal value suitable for writing to the pfSense configuration.
     */
    public function to_internal() : array {
        # Variables
        $internal_object = [];

        # Loop through each of this Model's fields and add its value to a serializable array
        foreach ($this->get_fields() as $field) {
            # Don't add this field if it's value is `null`. This is how values are unset.
            if (is_null($this->$field->value)) {
                continue;
            }

            # Init the namespace for this field if a namespace is specified and does not exist yet.
            if ($this->$field->internal_namespace and !array_key_exists($this->$field->internal_namespace, $internal_object)) {
                $internal_object[$this->$field->internal_namespace] = [];
            }

            # Nest this field under the namespace if specified and continue through the loop.
            if ($this->$field->internal_namespace) {
                $internal_object[$this->$field->internal_namespace][$this->$field->internal_name] = $this->$field->to_internal();
                continue;
            }

            # Otherwise, just store the field directly to the object
            $internal_object[$this->$field->internal_name] = $this->$field->to_internal();
        }

        return $internal_object;
    }

    /**
     * Obtain this Model object from the internal pfSense configuration by object ID. If the specified ID exists in
     * config, this Model object will be overwritten with the contents of that object.
     * @param mixed $id Specify the ID of the object to obtain.
     * @throws ServerError When this Model does not have a `config_path` set.
     * @throws NotFoundError When an object with the specified $id does not exist.
     */
    public function from_internal(mixed $id = null) {
        # Throw an error if no `config_path` is set
        if (!$this->config_path) {
            throw new ServerError(
                message: "Model must have a 'config_path' to get internal values from config.",
                ref_id: "MODEL_NO_CONFIG_PATH"
            );
        }

        # Require an $id if `many` is set
        if ($this->many and !isset($id)) {
            throw new ServerError(
                message: "An ID is required when refreshing objects from config on model with 'many'.",
                ref_id: "MODEL_ID_REQUIRED_FOR_REFRESH"
            );
        }

        # Get the internal object(s) from config
        $internal_objects = $this->get_config($this->config_path, []);

        # If an ID was requested that doesn't exist, throw a not found error
        if ($this->many and !array_key_exists($id, $internal_objects)) {
            throw new NotFoundError(
                message: "Object with ID '$id' does not exist.",
                ref_id: "MODEL_OBJECT_NOT_FOUND"
            );
        }

        # Set the object ID if `many` is enabled
        $this->id = ($this->many) ? $id : $this->id;

        # Fetch the requested object if `many` is enabled
        $internal_object = ($this->many) ? $internal_objects[$id] : $internal_objects;

        # Loop through each of this Model's fields and convert the internal value to the representation value
        foreach ($this->get_fields() as $field) {
            # Assign names and model context to this field
            $this->$field->context = $this;
            $this->$field->name = $field;
            $this->$field->check_field_names();

            # Check the internal object for this field directly and convert it's value from internal to representation
            if (!$this->$field->internal_namespace and array_key_exists($this->$field->internal_name, $internal_object)) {
                $this->$field->from_internal($internal_object[$this->$field->internal_name]);
            }

            # Check if this field has a namespace and check if the internal object has the namespace
            if ($this->$field->internal_namespace and array_key_exists($this->$field->internal_namespace, $internal_object)) {
                # Check the namespace for the field and convert it's value from internal to representation
                if (array_key_exists($this->$field->internal_name, $internal_object[$this->$field->internal_namespace])) {
                    $this->$field->from_internal(
                        $internal_object[$this->$field->internal_namespace][$this->$field->internal_name]
                    );
                }
            }
        }
    }

    /**
     * Fetches Model objects for all objects stored in the internal pfSense values. If `config_path` is set, this will
     * create Model objects for each object stored at the config path. If `internal_callable` is set, this will create
     * Model objects for each object returned by the specified callable.
     * @return array|Model Returns a ModelSet of Models if `many` is enabled or a single Model object if `many` is
     * not enabled.
     */
    public function get_all_from_internal() : ModelSet|Model {
        # Variables
        $model = get_class($this);
        $model_objects = [];

        # Throw an error if both `config_path` and `internal_callable` are set.
        if ($this->config_path and $this->internal_callable) {
            throw new ServerError(
                message: "Models cannot define both a config path and an internal callable.",
                ref_id: "MODEL_WITH_CONFIG_PATH"
            );
        }
        # Obtain the internal objects from the config path if specified
        elseif ($this->config_path) {
            $internal_objects = $this->get_config($this->config_path, []);
        }
        # Obtain the internal objects by calling the `internal_callable` if specified
        elseif ($this->internal_callable) {
            $callable = $this->internal_callable;
            $internal_objects = $callable();
        }
        # Otherwise, throw an error. Either a `config_path` or an `internal_callable` is required.
        else {
            throw new ServerError(
                message: "Model requires a 'config_path' or 'internal_callable' value to be defined before 
                    obtaining internal objects.",
                ref_id: "MODEL_WITH_NO_INTERNAL_METHOD"
            );
        }

        # Ensure internal objects are of type `array`
        if (!is_array($internal_objects)) {
            throw new ServerError(
                message: "Model expected internal objects to be of type 'array'.",
                ref_id: "MODEL_INVALID_INTERNAL_OBJECT_TYPE"
            );
        }

        # For non `many` models, wrap the internal object in an array so we can loop
        $internal_objects = ($this->many) ? $internal_objects : [$internal_objects];

        # Loop through each internal object and create a Model object for it
        foreach ($internal_objects as $internal_id=>$internal_object) {
            $model_object = new $model();
            $model_object->from_internal($internal_id);
            $model_objects[] = $model_object;
        }

        # Unwrap the array for non `many` models, otherwise return all objects
        return ($this->many) ? new ModelSet($model_objects) : $model_objects[0];
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

    public function from_representation(array $representation_data) {
        # When an ID is provided, fetch this objects from the internal configuration using the ID.
        if (array_key_exists("id", $representation_data)) {
            $this->from_internal($representation_data["id"]);
        }

        # Loop through each field in this Model and assign their values using the `representation_data`.
        foreach ($this->get_fields() as $field) {
            # Ensure a value for this field is in the representation data before continuing.
            if (array_key_exists($field, $representation_data)) {
                $this->$field->value = $representation_data[$field];
            }
        }
    }

    /**
     * Populates an array of field names for this model.
     * @return array An array of field names for this model.
     */
    final public function get_fields() : array {
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
    final public function validate() : bool {
        # Check for required packages
        $this->check_packages();

        # Loop through each of this object's assigned fields and validate them.
        foreach ($this->get_fields() as $field) {
            # Assign the name of this field and context to this Field object
            $this->$field->name = $field;
            $this->$field->internal_name = ($this->$field->internal_name) ?: $this->$field->name;
            $this->$field->context = $this;

            # Ensure this field's internal name is unique to its namespace
            $this->is_fields_internal_name_unique($field);

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
     * Checks to ensure each of this Model's Field object has an internal name that is unique to it's namespace.
     * @param string $field The name of the field to check for a unique internal name
     * @throws ServerError When this Model contains a Field object with an internal name that is not unique to it's
     * namespace.
     */
    private function is_fields_internal_name_unique(string $field) {
        # Loop to compare each field's names against each other to ensure there are no conflicts
        foreach ($this->get_fields() as $eval_field) {
            # Ignore if we are evaluating the same field, they will always have the same names.
            if ($field === $eval_field) {
                continue;
            }

            # Throw an error if the namespace and the internal name are the same. These must be unique.
            if ($this->$field->internal_namespace === $this->$eval_field->internal_namespace) {
                if ($this->$field->internal_name === $this->$eval_field->internal_name) {
                    throw new ServerError(
                        message: "Model field internal names must be unique to it's namespace.",
                        ref_id: "MODEL_FIELDS_WITH_CONFLICTING_INTERNAL_NAMES"
                    );
                }
            }
        }
    }

    /**
     * Checks if the required pfSense packages for this model are installed.
     * @throws NotFoundError When the Model requires a pfSense package that is not installed.
     * @throws ServerError When a package requires a PHP include file that could not be found.
     */
    private function check_packages() {
        # Loop through each required package and ensure it is present on the system.
        foreach ($this->packages as $pkg) {
            # Return an error if the package is not installed
            if (!is_pkg_installed($pkg)) {
                throw new NotFoundError(
                    message: "The requested action requires the '$pkg' package but it is not installed.",
                    ref_id: "MDOEL_MISSING_REQUIRED_PACKAGE"
                );
            }
        }

        # Loop through each package dependency and add it.
        foreach ($this->package_includes as $include) {
            if (!include_once($include)) {
                throw new ServerError(
                    message: "Model requires include file $include but it could not be found in the PHP path.",
                    ref_id: "MODEL_WITH_FAILED_INCLUDE"
                );
            }

        }
    }

    /**
     * Initializes the default 'apply' method. This method is intended to be overridden by a child model class and is
     * responsible for reloading/restarting services after changes occur.
     */
    protected function apply() {
    }

    /**
     * Initializes the default 'read' method. By default, this method will return the representation form of the current
     * Model object. Optionally if $all is enabled and this is a `many` model, this will return the representation
     * form of ALL Model objects from the internal config/callable.
     * @return array Returns the representation of the current Model object if $all is `false`. If $all is `true` and
     * this Model is a `many` model, this returns a representation of ALL model objects from the internal config or
     * callable.
     */
    final public function read(bool $all=false) : array {
        if ($all and $this->many) {
            return $this->get_all_from_internal();
        }

        return $this->to_representation();
    }

    /**
     * Initializes the default '_create' method. For models with a `config_path` and `many` set, this method will
     * automatically obtain the next model ID for the object and write this object to the pfSense config. Other models
     * will require this method to be overridden to support creations for this object.
     * @throws ServerError When the Model requesting creation does not have `many` enable and/or does not have a
     * `config_path` set. These Models' child class must override this method to add context-specific creations.
     */
    protected function _create() {
        # Check if this model has a `config_path` and `many` set, we will apply a default behavior for these models
        if ($this->config_path and $this->many) {
            # Get the next ID and name of this model
            $this->id = $this->get_next_id();
            $this->set_config(path: "$this->config_path/$this->id", value: $this->to_internal());
            $this->write_config("Added $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' or 'many' require an overridden _create method",
            ref_id: "MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD"
        );
    }

    /**
     * Perform validation, create this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     */
    final public function create(bool $apply = false) {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate()) {
            # Run the _create method
            $this->_create();

            # Only apply this change immediately if requested
            if ($apply) {
                $this->apply();
            }
        }
    }

    /**
     * Initializes the default '_update' method. For models with a `config_path`, this method will automatically
     * write changes to the existing object in configuration. Other models must override this method with context
     * specific updates.
     * @throws ServerError When the Model requesting update does not have a `config_path` set. These Models' child
     * class must override this method to add context-specific updates.
     */
    protected function _update() {
        # Check if this model has a `config_path`, we will apply a default behavior for these models
        if ($this->config_path) {
            # Only use the ID in the config path if 'many' is enabled. Otherwise, just use the base config path.
            $update_config_path = ($this->many) ? "$this->config_path/$this->id" : $this->config_path;

            # Write the changes to the object in config.
            $this->set_config(path: $update_config_path, value: $this->to_internal());
            $this->write_config("Modified $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' require an overridden _update method",
            ref_id: "MODEL_REQUIRE_OVERRIDDEN_UPDATE_METHOD"
        );
    }

    /**
     * Perform validation, update this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     */
    final public function update(bool $apply = false) {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate()) {
            # Run the _update method
            $this->_update();

            # Only apply this change immediately if requested
            if ($apply) {
                $this->apply();
            }
        }
    }

    /**
     * Initializes the default '_delete' method. For models with a `config_path` and `many`, this method will
     * automatically remove the existing object from configuration. Other models must override this method with
     * context-specific deletions.
     * @throws ServerError When the Model requesting delete does not have a `config_path` or `many` set. These Models'
     * child class must override this method to add context-specific deletions.
     */
    protected function _delete() {
        # Check if this model has a `config_path`, we will apply a default behavior for these models
        if ($this->config_path and $this->many) {
            # Write the changes to the object in config.
            $this->del_config(path: "$this->config_path/$this->id");
            $this->write_config("Deleted $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' require an overridden _delete method",
            ref_id: "MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD"
        );
    }

    /**
     * Perform validation, delete this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     */
    final public function delete(bool $apply = false) {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate()) {
            # Run the _delete method
            $this->_delete();

            # Only apply this change immediately if requested
            if ($apply) {
                $this->apply();
            }
        }
    }
}
