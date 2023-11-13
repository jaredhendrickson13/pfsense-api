<?php

namespace API\Fields;

require_once("api/auto_loader.inc");

use API;
use API\Core\Field;
use API\Core\Model;
use API\Responses\NotFoundError;
use API\Responses\ServerError;

/**
 * Defines a ForeignModelField class that extends the core Field method. This Field adds validation and representation
 * fields that relate to a different Model object field. For example, a ForeignModelField can be used to relate a static
 * route to its parent Gateway model object. 
 */
class ForeignModelField extends Field {
    public Model $model;
    public string $model_name;
    public array $model_query;
    public string $model_field;
    public string $model_field_internal;

    /**
     * Defines the ForeignModelField object and sets its options.
     * @param string $model_name The name of the foreign Model class this field relates to. This should be the name of an
     * existing Model class in the \API\Models namespace, but should not include the \API\Models namespace prefix. 
     * @param string $model_field The field on the assigned $model_name class that this field's value relates to. For
     * example, if this fields value must be an existing Gateway model object's `name`, you would set $model_name to
     * `Gateway` and $model_field to `name`.
     * @param string $model_field_internal In the event that you want to store a different field value internally,
     * specify that field name here. Leave empty to use the $model_field for both the internal and representation values.
     * For example, if this field's value must be set in config as an existing Gateway model object's `name`, but you 
     * want clients to reference the Gateway model object by its `id` instead, you would set $model_field to `id` and 
     * $model_field_internal to `name`.
     * @param array $model_query Enter query parameters to limit which of the $model's existing objects can be
     * referenced by this field. For example, say this field's value must be set to an existing FirewallAlias object's
     * `name`, but you only want to allow `port` type aliases; you would set $model_name to `FirewallAlias`, $model_field to
     * `name`, and $model_query to `["type" => "port"]`. Defaults to all existing $model objects.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param array $choices An array of value choices this Field can be assigned. This can either be an indexed array
     * of the exact choice values, or an associative array where the array key is the exact choice value and the array
     * value is a verbose name for the choice. Verbose choice name are used by ModelForms when generating web pages
     * for a given Model.
     * @param string $choices_callable Assign a callable method from this Field object OR the parent Model context to
     * execute to populate choices for this field. This callable must be a method assigned on this Field object OR the
     * parent Model object that returns an array of valid choices in the same format as $choices. This is helpful when
     * choices are dynamic and must be populated at runtime instead of pre-determined sets of values.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $editable Set to `false` to prevent this field's value from being changed after its initial creation.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
     * @param bool $write_only Set the `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param bool $representation_only Set to `true` to make this field only present in its representation form. This
     * effectively prevents the Field from being converted to an internal value which is saved to the pfSense config.
     * This should only be used for fields that do not relate directly to a configuration value.
     * @param bool $many If `true`, the value must be an array of many strings.
     * @param int $many_minimum When $many is set to `true`, this sets the minimum number of array entries required.
     * @param int $many_maximum When $many is set to `true`, this sets the maximum number of array entries allowed.
     * @param string|null $delimiter Assigns the string delimiter to use when writing array values to config.
     *  Use `null` if this field is stored as an actual array in config. This is only available if $many is set to
     *  `true`. Defaults to `,` to store as comma-separated string.
     * @param string $verbose_name The detailed name for this Field. This name will be used in non-programmatic areas
     * like web pages and help text. This Field will default to property name assigned to the parent Model with
     * underscores converted to spaces.
     * @param string $verbose_name_plural The plural form of $verbose_name. This defaults to $verbose_name with `s`
     * suffixed or `es` suffixes to strings already ending with `s`.
     * @param string $internal_name Assign a different field name to use when referring to the internal field as it's
     * stored in the pfSense configuration.
     * @param string $internal_namespace Sets the namespace this field belongs to internally. This can be used to nest
     * the fields internal value under a specific namespace as an associative array. This only applies to the internal
     * value, not the representation value.
     * @param array $referenced_by An array that specifies other Models and Field's that reference this Field's parent
     * Model using this Field's value. This will prevent the parent Model object from being deleted while it is actively
     * referenced by another Model object. The array key must be the name of the Model class that references this Field,
     * and the value must be a Field within that Model. The framework will automatically search for any existing Model
     * objects that have the referenced Field assigned a value that matches this Field's value.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other fields within the parent Model context. For example, if the parent Model context has two
     * fields, one field named `type` and the other being this field; and you only want this field to be included if
     * `type` is equal to `type1`, you could assign ["type" => "type1"] to this parameter.
     * @param array $validators An array of Validator objects to run against this field.
     * @param string $help_text Set a description for this field. This description will be used in API documentation.
     */
    public function __construct(
        string $model_name,
        string $model_field,
        string $model_field_internal = "",
        array $model_query = [],
        bool $required = false,
        bool $unique = false,
        mixed $default = null,
        array $choices = [],
        string $choices_callable = "",
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $editable = true,
        bool $read_only = false,
        bool $write_only = false,
        bool $representation_only = false,
        bool $many = false,
        int $many_minimum = 1,
        int $many_maximum = 128,
        string|null $delimiter = ",",
        string $verbose_name = "",
        string $verbose_name_plural = "",
        string $internal_name = "",
        string $internal_namespace = "",
        array $referenced_by = [],
        array $conditions = [],
        array $validators = [],
        string $help_text = ""
    )
    {
        # Assign properties unique to this Field object
        $model_name = "\\API\\Models\\$model_name";
        $this->model_query = $model_query;
        $this->model_field = $model_field;
        $this->model_field_internal = ($model_field_internal) ?: $model_field;
        
        # Ensure the properties assigned are allowed and that the assigned $model_name can be constructed
        $this->check_construct($model_name);

        # Construct the parent Field object
        parent::__construct(
            type: ($model_field === "id") ? "integer" : $this->model->$model_field->type,
            required: $required,
            unique: $unique,
            default: $default,
            choices: $choices,
            choices_callable: $choices_callable,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            editable: $editable,
            read_only: $read_only,
            write_only: $write_only,
            representation_only: $representation_only,
            many: $many,
            many_minimum: $many_minimum,
            many_maximum: $many_maximum,
            delimiter: $delimiter,
            verbose_name: $verbose_name,
            verbose_name_plural: $verbose_name_plural,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace,
            referenced_by: $referenced_by,
            conditions: $conditions,
            validators: $validators,
            help_text: $help_text
        );
    }

    /**
     * Checks that the object to be constructed has no conflicts.
     * @param string $model_name The foreign Model class's FQN. 
     */
    private function check_construct(string $model_name) {
        # Ensure the assigned $model_name is an existing Model class
        if (!class_exists($model_name)) {
            throw new ServerError(
                message: "ForeignModelField's `model_name` property musted be an existing Model class FQN, ".
                    "received `$model_name`.",
                response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_NAME" 
            );
        }
        
        # Create a new object using the assigned foreign Model class
        $this->model = new $model_name();
        
        # Ensure the foreign Model class is a `many` Model
        if (!$this->model->many) {
            throw new ServerError(
                message: "ForeignModelField `model_name` must be a Model class with `many` enabled.",
                response_id: "FOREIGN_MODEL_FIELD_ASSIGNED_NON_MANY_MODEL"
            );
        }
        
        # Ensure the `model_field` exists on this Model class
        if (!in_array($this->model_field, $this->model->get_fields()) and $this->model_field !== "id") {
            throw new ServerError(
                message: "ForeignModelField's `model_field` does not exist in class `$model_name`.",
                response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD"
            );
        }

        # Ensure the `model_field` is unique
        if ($this->model_field !== "id" and !$this->model->{$this->model_field}->unique) {
            throw new ServerError(
                    message: "ForeignModelField's `model_field` must be a `unique` Field.",
                response_id: "FOREIGN_MODEL_FIELD_WITH_NON_UNIQUE_MODEL_FIELD"
            );
        }

        # Ensure the `model_field_internal` exists on this Model class
        if (!in_array($this->model_field_internal, $this->model->get_fields()) and $this->model_field_internal !== "id") {
            throw new ServerError(
                message: "ForeignModelField's `model_field_internal` does not exist in class `$model_name`.",
                response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD_INTERNAL"
            );
        }

        # Ensure the `model_field_internal` is unique
        if ($this->model_field_internal !== "id" and !$this->model->{$this->model_field_internal}->unique) {
            throw new ServerError(
                message: "ForeignModelField's `model_field_internal` must be a `unique` Field.",
                response_id: "FOREIGN_MODEL_FIELD_WITH_NON_UNIQUE_MODEL_FIELD_INTERNAL"
            );
        }
    }

    /**
     * Converts the field value to its representation form from it's internal pfSense configuration value.
     * @param mixed $internal_value The internal value from the pfSense configuration.
     * @return string The field value in its representation form.
     */
    protected function _from_internal(mixed $internal_value) : string {
        # Add the query parameters needed to locate the related Model object
        $query_params = array_merge($this->model_query, [$this->model_field_internal => $internal_value]);
        
        # Query for the Model object this value relates to.
        $query_modelset = $this->model->query($query_params);
        
        # If the model object exists, return the `model_field` value.
        if ($query_modelset->exists()) {
            return $query_modelset->first()->{$this->model_field}->value;
        }
        
        # As a failsafe, return the existing internal value if we could not find the related object
        return $internal_value;
    }

    /**
     * Converts the represented value into the internal pfSense value.
     * @param mixed $representation_value The value to convert into it's internal form.
     * @return array|string|null The internal value(s) suitable for writing to the pfSense configuration.
     */
    protected function _to_internal(mixed $representation_value) : array|string|null {
        # Add the query parameters needed to locate the related Model object
        $query_params = array_merge($this->model_query, [$this->model_field => $representation_value]);

        # Query for the Model object this value relates to.
        $query_modelset = $this->model->query($query_params);

        # If the model object exists, return the existing `model_field_internal` value.
        if ($query_modelset->exists()) {
            return $query_modelset->first()->{$this->model_field_internal}->value;
        }

        # As a failsafe, return the existing representation value if we could not find the related object
        return (string)$representation_value;
    }

    /**
     * Checks that the value assigned to this Field corresponds to an existing foreign model object during validation.
     * @param mixed $value The value being validated. In the event that this is a `many` field, this method will
     * receive each value of the array individually, not the array value itself.
     */
    public function validate_extra(mixed $value) {
        # Add the query parameters needed to locate the related Model object
        $query_params = array_merge($this->model_query, [$this->model_field => $value]);

        # Query for the Model object this value relates to.
        $query_modelset = $this->model->query($query_params);
        
        # If the model object exists, return the existing `model_field_internal` value.
        if (!$query_modelset->exists()) {
            throw new NotFoundError(
                message: "Field `$this->name` could not locate `{$this->model->verbose_name}` object with ".
                    "`$this->model_field` set to `$value`",
                response_id: "FOREIGN_MODEL_FIELD_VALUE_NOT_FOUND"
            );
        }
        
    }
}
