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

require_once("api/auto_loader.inc");

use API;
use API\Responses\ConflictError;
use API\Responses\NotFoundError;
use API\Responses\ServerError;
use API\Responses\ValidationError;
use ReflectionClass;

# Constants
const DEFAULT_CLIENT_USERNAME = "(system api)";
const DEFAULT_CLIENT_IP_ADDRESS = "127.0.0.1";

/**
 * Creates a Model object that relates to a pfSense configuration or service. This model defines the necessary steps
 * to validate incoming configuration and create, read, update, and/or delete data from the pfSense configuration this
 * object relates to.
 */
class Model {
    public string|null $config_path = null;
    public bool $many = false;
    public mixed $id = null;
    public mixed $parent_id = null;
    public Model|null $parent_model = null;
    public string $parent_model_class = "";
    public Auth $client;
    public Model|null $initial_object = null;
    public array $packages = [];
    public array $package_includes = [];
    public array $unique_together_fields = [];
    public string $internal_callable = "";
    public int|null $sort_option = null;
    public string|null $sort_by_field = null;
    public string $subsystem = "";
    public bool $always_apply = false;
    public string $update_strategy = "merge";
    public string $verbose_name = "";
    public string $verbose_name_plural = "";

    /**
     * Values to assign when a Model object is first constructed.
     * @param mixed $id If specified, the internal object with this ID will be obtained as the Model object.
     * @param mixed|null $parent_id The ID of the parent object this object is nested under. This is only applicable
     * to Models with a `parent_model_class` assigned that is also a `many` Model.
     * @param mixed $data Allows objects to be obtained, created and/or updated using an array of
     * field-value pairs. If this array contains an `id` field, the internal object with this ID will be obtained as
     * the Model object first. Any additional fields will be used to set the Model object's values.
     * @param Auth|null $client Optionally assign an API client's `Auth` object. Required for tying users to
     * changes made through Models.
     */
    public function __construct(mixed $id=null, mixed $parent_id=null, mixed $data=[], Auth $client=null) {
        # Throw an error if both $id and data are set.
        if (isset($id) and $data) {
            throw new ServerError(
                message: "Model objects cannot be constructed with both an ID and representation data.",
                response_id: "MODEL_WITH_ID_AND_REPRESENTATION"
            );
        }

        # Ensure the representation data is an array
        if (!is_array($data)) {
            throw new ValidationError(
                message: "Representation data must be of type `array`.",
                response_id: "MODEL_NON_ARRAY_REPRESENTATION_DATA"
            );
        }
        
        # Set the parent ID if given
        $this->parent_id = $parent_id;

        # Obtain the current object from internal if an ID was given or this is not a `many` model.
        if (isset($id) or !$this->many) {
            # Don't pull internal values unless there is a `config_path` or `internal_callable` defined.
            if ($this->config_path or $this->internal_callable) {
                $this->id = $id;
                $this->from_internal();
                $this->initial_object = unserialize(serialize($this));
            }
        }

        # Use the representation to obtain or construct this object if present.
        if ($data) {
            $this->from_representation($data);
        }

        # Allow an Auth object to be assigned the model. Allows models to view a client's IP address and username.
        if ($client) {
            $this->client = $client;
        }
        # Otherwise, define a default system client if one was not specified.
        else {
            $this->client = new Auth();
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

        # Assign names and model context to each field in this Model
        foreach ($this->get_fields() as $field) {
            $this->$field->set_names($field);
            $this->$field->context = $this;
        }
    }

    /**
     * Retrieves the full config path including any assigned parent Model paths.
     */
    public function get_config_path() : string|null {
        # Format the config path accordingly when a `parent_model_class` is assigned
        if ($this->parent_model) {
            # Do not allow a parent Model to be set if this Model has an `internal_callable` assigned
            if ($this->internal_callable) {
                throw new ServerError(
                    message: "Model ".get_called_class()." cannot assign `parent_model_class` $this->parent_model_class ".
                        "while also having an `internal_callable` assigned.",
                    response_id: "MODEL_CANNOT_HAVE_PARENT_MODEL_CLASS_AND_INTERNAL_CALLABLE"
                );
            }
            
            # Do not allow the parent Model to be a child of any other model
            if ($this->parent_model->parent_model_class) {
                throw new ServerError(
                    message: "Model ".get_called_class()." cannot assign `parent_model_class` $this->parent_model_class ".
                    "because itself has a `parent_model_class` {$this->parent_model->parent_model_class}",
                    response_id: "MODEL_PARENT_MODEL_CLASS_CANNOT_HAVE_PARENT"
                );
            }

            # Do not allow the parent Model to be assigned if itself does not have a config path
            if (!$this->parent_model->config_path) {
                throw new ServerError(
                    message: "Model ".get_called_class()." cannot assign `parent_model_class` $this->parent_model_class ".
                    "because it does not have a `config_path` set.",
                    response_id: "MODEL_PARENT_MODEL_CLASS_MUST_HAVE_CONFIG_PATH"
                );
            }
            
            # Return a config path including the parent ID if the parent Model is a `many` model
            if ($this->parent_model->many) {
                return "{$this->parent_model->config_path}/$this->parent_id/$this->config_path";
            }
            
            # Otherwise, just prefix the parent Model's config path to this Model's config path
            return "{$this->parent_model->config_path}/$this->config_path";

        }
        
        # Just use this Model's config path if no parent Model is assigned
        return $this->config_path;
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
    public static function set_config(string $path, mixed $value, mixed $default=null) {
        global $config;
        return (array_set_path($config, $path, $value, $default));
    }

    /**
     * Merges changed Model field values into the config. This is similar to `set_config()` but instead of replacing
     * the entire config path with a set value, it only sets Field's known to this Model. Any fields in the stored
     * object that are not defined in a Field assigned to this Model will be left unchanged.
     * @param $path string The Model config path including any Model ID
     */
    public function merge_config(string $path) {
        # Loop through each field known to this Model
        foreach ($this->get_fields() as $field) {
            # Determine the field path
            if ($this->$field->internal_namespace) {
                $field_path = "$path/{$this->$field->internal_namespace}/{$this->$field->internal_name}";
            }
            else {
                $field_path = "$path/{$this->$field->internal_name}";
            }

            # Do not merge `representation_only` fields
            if ($this->$field->representation_only) {
                continue;
            }

            # Remove config values to fields that are `null`
            if ($this->$field->to_internal() === null) {
                $this->del_config(path: $field_path);
                continue;
            }

            # Otherwise, set the value normally
            $this->set_config(path: $field_path, value: $this->$field->to_internal());
        }
    }

    /**
     * Delete a key from the configuration
     * @param $path string path with '/' separators
     * @returns array copy of the removed value or null
     */
    public static function del_config(string $path) : mixed {
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
    public static function is_config_enabled(string $path, string $enable_key="enable") : bool {
        global $config;
        return (array_path_enabled($config, $path, $enable_key));
    }

    /**
     * Default method for obtaining the next ID from the pfSense configuration. This searches for the next index ID for
     * the model in the pfSense configuration and requires the model's `config_path` and `many` properties to be set.
     * For models that do not use a `config_path` or does not use config index IDs, this method can be overridden by
     * the child model class to add custom processes to obtain the next available ID.
     * @throws ServerError When the path leads to an existing non-empty, non-sequential array value
     * @returns string|int the next array key (ID) for the config path specified.
     */
    public function get_next_id() : string|int {
        # Since this references config IDs, this method requires `config_path` to be set. Throw an error if it's not.
        if (!$this->config_path) {
            throw new ServerError(
                message: "Model cannot use default method to obtain next available ID without 'config_path'",
                response_id: "MODEL_NEXT_ID_WITHOUT_CONFIG_PATH"
            );
        }

        # IDs are only applicable for `many` Models. Throw an error if `many` is not set.
        if (!$this->many) {
            throw new ServerError(
                message: "Models cannot obtain next ID when 'many' is not enabled.",
                response_id: "MODEL_NEXT_ID_WITHOUT_MANY"
            );
        }

        # First, get the configuration at the specific path
        $conf = $this->get_config($this->get_config_path(), []);

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
                response_id: "MODEL_DANGEROUS_CONFIG_PATH"
            );
        }
    }

    /**
     * Write configuration changes to the config file
     * @param string $change_note The message to write to the change log.
     */
    final protected function write_config(string $change_note) {
        # Local variables
        $lock_file = "/tmp/.api.write_config.lock";

        # Ensure there is not an API config lock, loop until the lock is released
        foreach (range(0, 60) as $attempt) {
            # Only write to the config if there is not an active lock
            if (!file_exists($lock_file)) {
                # Create a lock on API config writes while we write changes
                file_put_contents($lock_file, microtime());

                # Start a temporary session to write the config that contains the user's username in the config history
                $_SESSION["Username"] = $this->client->username;
                write_config(sprintf(gettext(" ".$change_note)));

                # If a subsystem is specified for this Model, mark it as dirty
                if ($this->subsystem) {
                    mark_subsystem_dirty($this->subsystem);
                }

                # Remove the temporary session and write lock
                unlink($lock_file);
                return;
            }

            # Delay attempts by 1 second
            sleep(1);
        }
    }

    /**
     * Logs an error to the syslog.
     * @param string $message The error message to write to the syslog
     */
    public function log_error(string $message) {
        # Call the pfSense `log_error` function
        log_error($message);
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
            if (is_null($this->$field->value) or is_null($this->$field->to_internal())) {
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
     * Obtains all internal objects for this Model. When a `config_path` is specified, this method will obtain the
     * internal objects directly from config. When an `internal_callable` is assigned, this method will return
     * the output of the assigned callable.
     * @throws ServerError When neither a `config_path` nor a `internal_callable` are assigned to this model, OR both a
     * `config_path` and a `internal_callable` are assigned to this model
     * @return array The array of internal objects without any additional processing performed.
     */
    public function get_internal_objects() : array {
        # Throw an error if both `config_path` and `internal_callable` are set.
        if ($this->config_path and $this->internal_callable) {
            throw new ServerError(
                message: "Models cannot define both a config path and an internal callable.",
                response_id: "MODEL_WITH_CONFIG_PATH"
            );
        }
        # Obtain the internal objects from the config path if specified
        elseif ($this->config_path) {
            $internal_objects = $this->get_config($this->get_config_path(), []);
        }
        # Obtain the internal objects by calling the `internal_callable` if specified
        elseif ($this->internal_callable) {
            $callable = $this->internal_callable;
            $internal_objects = $this->$callable();
        }
        # Otherwise, throw an error. Either a `config_path` or an `internal_callable` is required.
        else {
            throw new ServerError(
                message: "Model requires a 'config_path' or 'internal_callable' value to be defined before 
                    obtaining internal objects.",
                response_id: "MODEL_WITH_NO_INTERNAL_METHOD"
            );
        }

        return $internal_objects;
    }

    # TODO: Rename this `refresh_from_internal()`
    /**
     * Obtain this Model object from the internal pfSense configuration by object ID. If the specified ID exists in
     * config, this Model object will be overwritten with the contents of that object.
     * @throws ServerError When this Model does not have a `config_path` set.
     * @throws NotFoundError When an object with the specified $id does not exist.
     */
    public function from_internal() {
        # Require a `parent_id` if a `many` parent Model is assigned
        if ($this->parent_model_class and $this->parent_model->many and !isset($this->parent_id)) {
            throw new ServerError(
                message: "A parent ID is required when refreshing objects from config on model with a 'many' parent.",
                response_id: "MODEL_PARENT_ID_REQUIRED_FOR_REFRESH"
            );
        }
        
        # Require an $id if `many` is set
        if ($this->many and !isset($this->id)) {
            throw new ServerError(
                message: "An ID is required when refreshing objects from config on model with 'many'.",
                response_id: "MODEL_ID_REQUIRED_FOR_REFRESH"
            );
        }

        # Get the internal object(s) from config and keep track of fields with custom from internal methods
        $internal_objects = $this->get_internal_objects();

        # Require there to be an internal object with this ID, throw a not found error otherwise
        if ($this->many and !array_key_exists($this->id, $internal_objects)) {
            throw new NotFoundError(
                message: "Object with ID '$this->id' does not exist.",
                response_id: "MODEL_OBJECT_NOT_FOUND"
            );
        }

        # When `many` is enabled, obtain the object with our ID. Otherwise, just assign the direct object
        $internal_object = ($this->many) ? $internal_objects[$this->id] : $internal_objects;

        $this->from_internal_object($internal_object);
    }

    public function from_internal_object(array $internal_object) {
        # Variables
        $custom_from_internal_fields = [];

        # Loop through each of this Model's fields and convert the internal value to the representation value
        foreach ($this->get_fields() as $field) {
            # Check if a custom from internal method was implemented for this model field.
            if (method_exists($this, "from_internal_$field")) {
                # Capture fields with custom from internal methods. We will run them after this loop completes.
                $custom_from_internal_fields[] = $field;
            }

            # Check the internal object for this field directly and convert it's value from internal to representation
            if (!$this->$field->internal_namespace) {
                $this->$field->from_internal($internal_object[$this->$field->internal_name]);
            }

            # Check if this field has a namespace and check if the internal object has the namespace
            if ($this->$field->internal_namespace and array_key_exists($this->$field->internal_namespace, $internal_object)) {
                # Ensure the internal namespace is an array if the namespace is empty
                if (!$internal_object[$this->$field->internal_namespace]) {
                    $internal_object[$this->$field->internal_namespace] = [];
                }

                # Check the namespace for the field and convert it's value from internal to representation
                if (array_key_exists($this->$field->internal_name, $internal_object[$this->$field->internal_namespace])) {
                    $this->$field->from_internal(
                        $internal_object[$this->$field->internal_namespace][$this->$field->internal_name]
                    );
                }
            }
        }

        # Run any custom from internal methods found during the loop.
        foreach ($custom_from_internal_fields as $field) {
            $custom_from_internal_method = "from_internal_$field";
            $this->$field->value = $this->$custom_from_internal_method($this->$field->value);
        }
    }

    /**
     * Creates a serializable array representation of this object. This array is suitable for content
     * serialization such as JSON, YAML, etc.
     * @return array The serializable array representation of this object.
     */
    final public function to_representation() : array {
        # Variables
        $representation = [];

        # Embed this object's parent ID if set
        if (isset($this->parent_id)) {
            $representation["parent_id"] = $this->parent_id;
        }
        
        # Embed this object's ID if set
        if (isset($this->id)) {
            $representation["id"] = $this->id;
        }

        # Loop through each of this Model's fields and add its value to a serializable array
        foreach ($this->get_fields() as $field) {
            # Only add this field if it is not a `write_only` field.
            if (!$this->$field->write_only) {
                $representation[$field] = $this->$field->value;
            }
        }

        return $representation;
    }

    /**
     * Loads the Model object using an array representation of the object.
     * @param array $data The array of key-value pairs to use when loading the Model object from
     * representation.
     */
    final public function from_representation(array $data) {
        # Set the parent ID if provided
        if (array_key_exists("parent_id", $data)) {
            $this->parent_id = $data["parent_id"];
        }
        
        # When an ID is provided, fetch this objects from the internal configuration using the ID.
        if (array_key_exists("id", $data)) {
            # Extract the ID from the data and obtain the parent model object if applicable
            $this->id = $data["id"];
            $this->get_parent_model();
            
            # Obtain the object from its internal form
            $this->from_internal();
            $this->initial_object = unserialize(serialize($this));
        }

        # Loop through each field in this Model and assign their values using the `representation_data`.
        foreach ($this->get_fields() as $field) {
            # Ensure a value for this field is in the representation data before continuing.
            if (array_key_exists($field, $data)) {
                $this->$field->value = $data[$field];
            }
        }
    }

    /**
     * Converts this Model object to a PHP array representation of its OpenAPI schema. This is used when
     * auto-generating API documentation. This method can be extended to add additional options to the OpenAPI
     * schema when necessary
     * @link https://swagger.io/docs/specification/data-models/
     * @return array A PHP array containing this Model as an OpenAPI schema.
     */
    public function to_openapi_schema() : array {
        # Assign primitive schema attributes
        $openapi_schema = ["type" => "object", "properties" => []];

        # Loop through each Field in this Model and obtain its OpenAPI property config
        foreach ($this->get_fields() as $field) {
            # Generate and add the OpenAPI property from the Field object
            $openapi_schema["properties"][$field] = $this->$field->to_openapi_property();
        }

        return $openapi_schema;
    }

    /**
     * Obtains the parent of this Model object if a parent Model is assigned.
     * @return Model|null The parent Model object with the provided `parent_id` or `null` if no parent Model is set.
     * @throws NotFoundError If the parent Model object does not exist.
     */
    public function get_parent_model() : Model|null
    {
        # If a parent Model is assigned, create the parent object
        if ($this->parent_model_class) {
            # Set the FQN parent class name
            $parent_model_class = "\\API\\Models\\$this->parent_model_class";
            
            # Throw an error if the given parent ID does not exist
            try {
                $this->parent_model = new $parent_model_class(id: $this->parent_id);
            }
            catch (NotFoundError) {
                # Throw a NotFoundError but change some of the verbage
                throw new NotFoundError(
                    message: "Parent object with ID '$this->id' does not exist.",
                    response_id: "MODEL_PARENT_OBJECT_NOT_FOUND"
                );
            }
            
            # Set the parent Model object and return it
            return $this->parent_model;
        }
        
        return null;
    }
        
       

    /**
     * Populates an array of field names for this model.
     * @param bool $required_only Only obtain required Fields
     * @return array An array of field names for this model.
     */
    final public function get_fields(bool $required_only = false) : array {
        # Variables
        $fields = [];

        # Loop through each var in this object and extract the objects with a base class of API\Core\Field.
        foreach (get_object_vars($this) as $var => $value) {
            # Skip this var if it's not an object
            if (!is_object($value)) {
                continue;
            }

            # Skip this item if it is not child class of the API\Core\Field class
            if (!in_array("API\Core\Field", class_parents($value))) {
                continue;
            }

            # Skip this item if we are only gathering required Fields and this isn't a required Field
            if ($required_only and !$this->$var->required) {
                continue;
            }

            # Assign the name of this field and context to this Field object
            $this->$var->name = $var;
            $this->$var->internal_name = ($this->$var->internal_name) ?: $this->$var->name;
            $this->$var->context = $this;

            # Otherwise, this is a valid Field object. Include it.
            $fields[] = $var;
        }

        return $fields;
    }

    /**
     * Checks that any fields which must be unique together are unique from all other existing objects for this Model.
     * @throws ServerError When the fields specified in `unique_together_fields` are not Field objects known to this
     * Model.
     * @throws ValidationError When the unique together fields are not unique and are present in another Model object.
     */
    private function check_unique_together() {
        # Skip this validation for non `many` Models or Models without unique together fields
        if (!$this->many or !$this->unique_together_fields) {
            return;
        }

        # Throw an error if Fields were provided that are not known to the Model
        foreach ($this->unique_together_fields as $unique_together_field) {
            if (!in_array($unique_together_field, $this->get_fields())) {
                throw new API\Responses\ServerError(
                    message: "Unknown Model field `$unique_together_field` specified in `unique_together_fields`",
                    response_id: "MODEL_UNIQUE_TOGETHER_FIELDS_WITH_UNKNOWN_FIELD"
                );
            }
        }

        # Use this variable to keep track of query parameters to use when checking uniqueness
        $query_params = [];

        # Exclude this Model object from the query if this Model has an ID
        if (isset($this->id)) {
            $query_params = ["id__except" => $this->id];
        }

        # Format the query parameters as an associative array
        foreach ($this->unique_together_fields as $field) {
            $query_params[$field] = $this->$field->value;
        }

        # Query for existing objects with the same Field values
        $model_query = $this->read_all()->query(query_params: $query_params);

        # If any existing Models were found matching the query, fields are not unique together, throw an error
        if ($model_query->model_objects) {
            $unique_fields_string = implode(", ", $this->unique_together_fields);
            $in_use_by_id = $model_query->model_objects[0]->id;
            throw new ValidationError(
                message: "Fields [$unique_fields_string] together must be unique from all other objects of its kind. ".
                    "Values in use by object with ID `$in_use_by_id`.",
                response_id: "MODEL_FIELDS_NOT_UNIQUE_TOGETHER"
            );
        }
    }

    /**
     * Performs a full validation on the API mode. This includes checking that required packages are installed and
     * validating all assigned fields. Note: Fields are validated in the order they are defined in the class.
     * @param bool $requires_id Whether the `id` field should be validated in this request. This is typically only
     * necessary for update and delete actions.
     * @param bool $only_id Only validate the `id` field. This is primarily used for deletions.
     * @param bool $skip_parent Skip validating the assigned parent ID. This only applies to Models that have a
     * `parent_model_class` value assigned. In most cases, this should not be changed from the default. This is
     * only intended for nested Model validation.
     * @return bool `true` if the model's data is valid.
     */
    final public function validate(bool $requires_id = false, bool $only_id = false, bool $skip_parent = false) : bool {
        # When an ID is required and this is a `many` model, check that an ID is set.
        if ($requires_id and $this->many and !isset($this->id)) {
            throw new ValidationError(
                message: "Field 'id' is required.",
                response_id: "MODEL_REQUIRES_ID"
            );
        }
        
        # Validate the `parent_id` field if it was not requested we skip this step.
        if (!$skip_parent) {
            # Require a parent ID if the parent model is a `many` model
            if ($this->parent_model_class and $this->parent_model->many and !isset($this->parent_id)) {
                throw new ValidationError(
                    message: "Field `parent_id` is required.",
                    response_id: "MODEL_PARENT_ID_REQUIRED"
                );
            }

            # Ensure the parent Model exists if configured
            $this->get_parent_model();
        }
        

        # Check for required packages
        $this->check_packages();
        
        # Don't validate field values if `only_id` is enabled
        if ($only_id) {
            return true;
        }

        # Use a variable to keep track of fields with custom validate methods
        $custom_validate_fields = [];

        # Loop through each of this object's assigned fields and validate them.
        foreach ($this->get_fields() as $field) {
            # Assign this object as the Fields parent Model context
            $this->$field->context = $this;

            # Ensure this field's internal name is unique to its namespace
            $this->is_fields_internal_name_unique($field);

            # Run validation on this field
            $this->$field->validate();

            # Check if a custom validation method was implemented for this model field. Skip if null.
            if (method_exists($this, "validate_$field") and !is_null($this->$field->value)) {
                # Store the custom validation field so we can call it afterward
                $custom_validate_fields[] = $field;
            }
        }

        # Loop through fields that had custom validation methods and call those methods.
        # Running these after the initial validation loop above allows all fields to be available to these methods.
        foreach ($custom_validate_fields as $field) {
            # Format the name of the validate method for this field so we can call the function by variable
            $custom_validate_method = "validate_$field";
            
            # Validate each value individually for `many` Fields
            if ($this->$field->many) {
                $value_array = [];
                foreach ($this->$field->value as $value) {
                    $value_array[] = $this->$custom_validate_method($value);
                }
                $this->$field->value = $value_array;
            }
            # Otherwise, just validate the primitive value
            else {
                $this->$field->value = $this->$custom_validate_method($this->$field->value);
            }
        }

        # Validate unique together fields
        $this->check_unique_together();

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
                        response_id: "MODEL_FIELDS_WITH_CONFLICTING_INTERNAL_NAMES"
                    );
                }
            }
        }
    }

    /**
     * Obtains external Models that reference this Model. This method checks Field objects assigned to this Model object
     * for the `referenced_by` property. Any Model objects found that reference this Model object will be returned in
     * a ModelSet object.
     * @return ModelSet A ModelSet containing any Model objects that reference this Model object.
     */
    public function get_model_references() : ModelSet {
        # Variables
        $references = [];

        # Loop through each field in this Model object and check for Fields with the `referenced_by` property.
        foreach ($this->get_fields() as $field) {
            # Skip this field if it doesn't have any Models specified in its `referenced_by` property
            if (!$this->$field->referenced_by) {
                continue;
            }

            # Loop through each Model specified in `referenced_by`
            foreach ($this->$field->referenced_by as $reference_model => $reference_fields) {
                # Ensure the reference fields are always an array and reference model is FQN
                $reference_fields = (is_array($reference_fields)) ? $reference_fields : [$reference_fields];
                $reference_model = "\\API\\Models\\$reference_model";

                # Loop through each referenced field and query for matches.
                foreach ($reference_fields as $reference_field) {
                    # Query for any referenced Model objects that have this field value assigned to the referenced field
                    $reference_model = new $reference_model();
                    $reference_query = $reference_model->read_all()->query(
                        query_params: [$reference_field => $this->$field->value]
                    );
                    $references = $references + $reference_query->model_objects;
                }
            }
        }

        return new ModelSet(model_objects: $references);
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
                    response_id: "MDOEL_MISSING_REQUIRED_PACKAGE"
                );
            }
        }

        # Loop through each package dependency and add it.
        foreach ($this->package_includes as $include) {
            if (!include_once($include)) {
                throw new ServerError(
                    message: "Model requires include file $include but it could not be found in the PHP path.",
                    response_id: "MODEL_WITH_FAILED_INCLUDE"
                );
            }

        }
    }

    /**
     * Initializes the default 'pre_apply' method. This method is intended to be overidden by a child model class and
     * is called immediately before the 'apply' method. This method runs regardless of whether an apply was requested.
     */
    protected function pre_apply() {
    }

    /**
     * Initializes the default 'apply' method. This method is intended to be overridden by a child model class and is
     * responsible for reloading/restarting services after changes occur.
     */
    public function apply() {
    }

    /**
     * Initializes the default 'pre_apply_create' method. This method is intended to be overridden by a child model class and
     * is called immediately before the 'apply' method for create actions only. This method runs regardless of whether
     * an apply was requested. By default, this method simply calls the global 'pre_apply' method.
     */
    protected function pre_apply_create() {
        $this->pre_apply();
    }

    /**
     * Performs steps and processes required to apply changes after the `create()` method runs. This can be overridden
     * to add custom steps applying for just create actions. Defaults to simply calling `apply()`.
     */
    protected function apply_create() {
        $this->apply();
    }

    /**
     * Initializes the default 'pre_apply_update' method. This method is intended to be overridden by a child model class and
     * is called immediately before the 'apply' method for update actions only. This method runs regardless of whether
     * an apply was requested. By default, this method simply calls the global 'pre_apply' method.
     */
    protected function pre_apply_update() {
        $this->pre_apply();
    }

    /**
     * Performs steps and processes required to apply changes after the `update()` method runs. This can be overridden
     * to add custom steps to apply after update actions only. Defaults to simply calling `apply()`.
     */
    protected function apply_update() {
        $this->apply();
    }

    /**
     * Initializes the default 'pre_apply_delete' method. This method is intended to be overridden by a child model class and
     * is called immediately before the 'apply' method for delete actions only. This method runs regardless of whether
     * an apply was requested. By default, this method simply calls the global 'pre_apply' method.
     */
    protected function pre_apply_delete() {
        $this->pre_apply();
    }

    /**
     * Performs steps and processes required to apply changes after the `delete()` method runs. This can be overridden
     * to add custom steps to apply after delete actions only. Defaults to simply calling `apply()`.
     */
    protected function apply_delete() {
        $this->apply();
    }

    /**
     * Performs steps and processes required to apply changes after the `replace_all()` method runs. This can be
     * overridden to add custom steps to apply after replace all actions only. Defaults to simply calling `apply()`.
     */
    protected function apply_replace_all() {
        $this->apply();
    }

    /**
     * Fetches Model objects for all objects stored in the internal pfSense values. If `config_path` is set, this will
     * load Model objects for each object stored at the config path. If `internal_callable` is set, this will create
     * Model objects for each object returned by the specified callable.
     * @return ModelSet|Model Returns a ModelSet of Models if `many` is enabled or a single Model object if `many` is
     * not enabled.
     */
    public static function read_all() : ModelSet|Model {
        # Variables
        $model_name = get_called_class();
        $model = new $model_name();
        $model_objects = [];
        $internal_objects = $model->get_internal_objects();

        # For non `many` models, wrap the internal object in an array so we can loop
        $internal_objects = ($model->many) ? $internal_objects : [$internal_objects];

        # Loop through each internal object and create a Model object for it
        foreach ($internal_objects as $internal_id=>$internal_object) {
            $model_object = new $model(id: $internal_id);
            $model_object->from_internal();
            $model_objects[] = $model_object;
        }

        # Unwrap the array for non `many` models, otherwise return all objects
        return ($model->many) ? new ModelSet($model_objects) : $model_objects[0];
    }

    /**
     * Sorts `many` Model entries internally before writing the changes to config. This is useful for Model's whose
     * internal objects must be written in a specific order.
     */
    protected function sort() {
        # Do not sort if there is no `sort_option` or `sort_by_field` set
        if (!$this->sort_option or !$this->sort_by_field) {
            return;
        }

        # Do not allow non `many` models to be sorted
        if (!$this->many) {
            throw new ServerError(
                message: "Only `many` Models can be sorted.",
                response_id: "MODEL_CANNOT_BE_SORTED_WITHOUT_MANY"
            );
        }
        
        $internal_objects = $this->get_config($this->get_config_path(), []);
        $criteria = [];

        # Loop through each rule and map its sort field value to our sort criteria array
        foreach ($internal_objects as $id=>$internal_object) {
            # Store the internal object's existing ID so we can locate new IDs after sorting
            $internal_objects[$id]["original_id"] = $id;
            
            # Use the `internal_name` of the assigned `sort_by_field` since we are dealing with internal objects
            $sort_by_field_internal_name = $this->{$this->sort_by_field}->internal_name;
            
            # Map the real field if it's not empty, otherwise assume an empty string
            if (!empty($internal_object[$sort_by_field_internal_name])) {
                $criteria[$id] = $internal_object[$sort_by_field_internal_name];
            } else {
                $criteria[$id] = "";
            }
        }
        
        # Sort the internal objects using the previously determined criteria
        array_multisort($criteria, $this->sort_option, $internal_objects);

        # Loop through the sorted internal objects and find $this object's new ID
        $id_found = false;
        foreach ($internal_objects as $new_id => $sorted_internal_object) {
            # Check if this sorted internal object contains $this objects original ID
            if (!$id_found and $this->id === $sorted_internal_object["original_id"]) {
                $id_found = true;
                $this->id = $new_id;
            }
            
            # Remove the `original_id` value so we don't save it to config
            unset($internal_objects[$new_id]["original_id"]);
        }
        
        # Sets the sorted internal objects to the pfSense config
        $this->set_config($this->get_config_path(), array_values($internal_objects));
    }

    /**
     * Performs a query on all Model objects for this Model. This is essentially a shorthand way of calling
     * `read_all()->query()`. This method is only applicable to `many` models.
     * @param array $query_params An array of query parameters.
     * @param array $excluded An array of field names to exclude from the query. This is helpful when
     * query data may have extra values that you do not want to include in the query.
     * @param mixed ...$vl_query_params Qyery parameters that are passed in using variable-length arguments
     * https://www.php.net/manual/en/functions.arguments.php#functions.variable-arg-list
     * @return ModelSet The queried ModelSet containing only Model objects that matched the query.
     */
    public static function query(array $query_params=[], array $excluded=[], ...$vl_query_params) : ModelSet {
        # Merge the $query_params and any provided variable-length arguments into a single variable
        $query_params = array_merge($query_params, $vl_query_params);
        
        return self::read_all()->query(query_params: $query_params, excluded: $excluded);
    }


    /**
     * Initializes the default 'read' method. By default, this method loads the Model object from its internal
     * object using the current $this->id value and returns the ($this) Model object. Running this method will overwrite
     * $this object.
     * @return Model Loads the Model object from its internal value using the current $this->id value and
     * returns that object.
     */
    final public function read() : Model {
        # When an ID is required and this is a `many` model, check that an ID is set.
        if ($this->many and !isset($this->id)) {
            throw new ValidationError(
                message: "Field 'id' is required.",
                response_id: "MODEL_REQUIRES_ID"
            );
        }
        
        # Ensure the parent Model exists if configured
        $this->get_parent_model();

        # Obtain the requested Model object from config by ID
        $this->from_internal();

        return $this;
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
            # Write the new object to the internal config
            $this->set_config(path: "{$this->get_config_path()}/$this->id", value: $this->to_internal());
            $this->sort();
            $this->write_config("Added $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' or 'many' require an overridden _create method",
            response_id: "MODEL_REQUIRE_OVERRIDDEN_CREATE_METHOD"
        );
    }

    /**
     * Perform validation, create this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     * @return Model Returns the created Model object (this object).
     */
    final public function create(bool $apply = false) : Model {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate()) {
            # Get the next available internal ID and assign it to this object for `many` models with a config path
            if ($this->config_path and $this->many) {
                $this->id = $this->get_next_id();
            }

            # Run the _create method
            $this->_create();

            # Mark the subsystem as dirty if set
            if ($this->subsystem) {
                mark_subsystem_dirty($this->subsystem);
            }

            # Run the pre-apply method
            $this->pre_apply_create();

            # Only apply this change immediately if requested
            if ($apply or $this->always_apply) {
                $this->apply_create();
                clear_subsystem_dirty($this->subsystem);
            }

            # Refresh the initial object
            $this->initial_object = unserialize(serialize($this));
        }

        # Return the current representation of this object
        return $this;
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
            $config_path = ($this->many) ? "{$this->get_config_path()}/$this->id" : $this->get_config_path();

            # Merge differences between the stored object and updated internal object when update strategy is 'merge'
            if ($this->update_strategy === "merge") {
                $this->merge_config($config_path);
            }
            # Replace the internal object entirely when update strategy is 'replace'
            elseif ($this->update_strategy === "replace") {
                $this->set_config(path: $config_path, value: $this->to_internal());

            }
            # Throw an error if the update strategy is unknown
            else {
                throw new ServerError(
                    message: "Model has unknown update strategy '$this->update_strategy'",
                    response_id: "MODEL_WITH_UNKNOWN_MERGE_STRATEGY"
                );
            }

            # Sort an write the changes to the object in config.
            $this->sort();
            $this->write_config("Modified $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' require an overridden _update method",
            response_id: "MODEL_REQUIRE_OVERRIDDEN_UPDATE_METHOD"
        );
    }

    /**
     * Perform validation, update this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     * @return Model Returns the updated Model object (this object).
     */
    final public function update(bool $apply = false) : Model {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate(requires_id: true)) {
            # Run the _update method
            $this->_update();

            # Mark the subsystem as dirty if set
            if ($this->subsystem) {
                mark_subsystem_dirty($this->subsystem);
            }

            # Run the pre-apply method
            $this->pre_apply_update();

            # Only apply this change immediately if requested
            if ($apply or $this->always_apply) {
                $this->apply_update();
                clear_subsystem_dirty($this->subsystem);
            }

            # Refresh the initial object
            $this->initial_object = unserialize(serialize($this));
        }

        # Return the current representation of this object
        return $this;
    }

    /**
     * Perform validation and replace all existing objects for this Model with a specified set of Model representations.
     * This method is only available for `many` models.
     * @param mixed $data An array of many Model representations to set. Any existing Model objects will
     * be replaced with the objects defined in this array.
     * @return ModelSet A ModelSet containing all the Model objects found in config after replacement.
     */
    final public function replace_all(mixed $data, bool $apply = false) : ModelSet {
        # Ensure this is a `many` model before continuing
        if (!$this->many) {
            throw new ServerError(
                message: "Model `replace_all()` method is only available for models with `many` enabled.",
                response_id: "MODEL_UPDATE_ALL_WITHOUT_MANY"
            );
        }

        # Ensure the representation data is an array
        if (!is_array($data)) {
            throw new ValidationError(
                message: "Representation data must be of type `array`.",
                response_id: "MODEL_NON_ARRAY_REPRESENTATION_DATA"
            );
        }

        # Empty the config array for this Model
        $this->set_config($this->get_config_path(), []);

        # Loop through each Model representation included in the array
        foreach ($data as $representation_object) {
            # Remove any ID for this object if it was passed in
            unset($representation_object["id"]);

            # Define a new Model object using this representation and validate
            $model_object = new $this(data: $representation_object);
            $model_object->validate();

            # If an error was not thrown during Model object creation or validation, give it an ID and set the config
            $model_object->id = $this->get_next_id();
            $this->set_config("{$this->get_config_path()}/$model_object->id", $model_object->to_internal());
        }

        # Sort and write the changes to config
        $this->sort();
        $this->write_config("Replaced all $this->verbose_name_plural via API");

        # Mark the subsystem as dirty if set
        if ($this->subsystem) {
            mark_subsystem_dirty($this->subsystem);
        }

        # Apply if requested
        if ($apply or $this->always_apply) {
            $this->apply_replace_all();
            clear_subsystem_dirty($this->subsystem);
        }

        return $this->read_all();
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
            $this->del_config(path: "{$this->get_config_path()}/$this->id");
            $this->write_config("Deleted $this->verbose_name via API");
            return;
        }

        # Otherwise, this method needs to be overridden by a child class to add specific context. Throw an error.
        throw new ServerError(
            message: "Models without a 'config_path' require an overridden _delete method",
            response_id: "MODEL_REQUIRE_OVERRIDDEN_DELETE_METHOD"
        );
    }

    /**
     * Perform validation, delete this object internally and restart any associated services.
     * @param bool $apply Set to `true` if you would like to immediately apply the changes after they are made.
     * @return Model Returns the deleted Model object (this object).
     * @throws ConflictError When this Model object is referenced by another existing Model object.
     */
    final public function delete(bool $apply = false) : Model {
        # Ensure all object fields and validations succeed for proceeding.
        if ($this->validate(requires_id: true, only_id: true)) {
            # Block the deletion if this Model is still referenced by other Model objects
            $referenced_by = $this->get_model_references();
            if ($referenced_by->model_objects) {
                throw new ConflictError(
                    message: "Object cannot be deleted because it is referenced by another existing object.",
                    response_id: "MODEL_CANNOT_BE_DELETED_WHILE_REFERENCED",
                    data: $referenced_by->to_representation()
                );
            }

            # Run the _delete method
            $this->_delete();

            # Mark the subsystem as dirty if set
            if ($this->subsystem) {
                mark_subsystem_dirty($this->subsystem);
            }

            # Run the pre-apply method
            $this->pre_apply_delete();

            # Only apply this change immediately if requested or configured to do so
            if ($apply or $this->always_apply) {
                $this->apply_delete();
                clear_subsystem_dirty($this->subsystem);
            }

            # Refresh the initial object
            $this->initial_object = unserialize(serialize($this));
        }

        # Return the current representation of this object
        return $this;
    }
}
