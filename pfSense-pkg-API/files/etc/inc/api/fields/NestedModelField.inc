<?php

namespace API\Fields;

use API\Core\Field;
use API\Core\Model;

class NestedModelField extends Field
{
    public string $model_class;
    
    /**
     * @param bool $required Set to `true` if this field is required to be explicitly set. $required and $default are
     * mutually exclusive.
     * @param mixed|null $default The default value to assign this field if no value was found. If not set, it assumed
     * this Field requires a value and should fail validation if no value was found.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null Set to `true` if `null` values should be supported for this field. When `null` values
     * are supported, the underlying pfSense configuration field will be removed entirely when receiving a null value.
     * @param bool $editable Set to `false` to prevent this field's value from being changed after its initial creation.
     * @param bool $read_only Set to `true` to make this field read-only. Attempts to write to a read-only field will
     * result in a thrown exception.
     * @param bool $write_only Set to `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param bool $representation_only Set to `true` to make this field only present in its representation form. This
     * effectively prevents the Field from being converted to an internal value which is saved to the pfSense config.
     * This should only be used for fields that do not relate directly to a configuration value.
     * @param string $verbose_name The detailed name for this Field. This name will be used in non-programmatic areas
     * like web pages and help text. This Field will default to property name assigned to the parent Model with
     * underscores converted to spaces.
     * @param string $verbose_name_plural The plural form of $verbose_name. This defaults to $verbose_name with `s`
     * suffixed or `es` suffixes to strings already ending with `s`.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other fields within the parent Model context. For example, if the parent Model context has two
     * fields, one field named `type` and the other being this field; and you only want this field to be included if
     * `type` is equal to `type1`, you could assign ["type" => "type1"] to this parameter.
     * @param string $help_text Sets a description for this field. This description will be used as the description
     * for this field in the API documentation.
     */
    public function __construct(
        string $model_class,
        bool $required = false, 
        mixed $default = null,
        bool $allow_empty = false, 
        bool $allow_null = false, 
        bool $editable = true, 
        bool $read_only = false, 
        bool $write_only = false, 
        bool $representation_only = false,
        string $verbose_name = "", 
        string $verbose_name_plural = "",
        array $conditions = [], 
        string $help_text = ""
    )
    {
        # Assign attributes specific to this Field
        $this->model_class = "\\API\\Models\\$model_class";
        
        # Construct the parent Field object with desired parameters.
        parent::__construct(
            type: "array",
            required: $required,
            default: $default,
            allow_empty: $allow_empty, 
            allow_null: $allow_null, 
            editable: $editable, 
            read_only: $read_only, 
            write_only: $write_only, 
            representation_only: $representation_only,
            delimiter: null, 
            verbose_name: $verbose_name, 
            verbose_name_plural: $verbose_name_plural,
            conditions: $conditions, 
            help_text: $help_text
        );
    }
    
    /**
     * Validates the incoming value using the assigned Model's `validate()` method.
     */
    public function validate_extra(mixed $value)
    {
        # Create a new Model object for this value and validate it
        $model = new $this->model_class(data: $value);
        $model->validate(skip_parent: true);
        
        return $value;
    }

    /**
     * Converts the value from its representation value to its internal value.
     * @param mixed $representation_value The representation value to be converted to its internal equivalent
     * @return array|string|null The internal value to be stored in the pfSense config
     */
    public function _to_internal(mixed $representation_value): array|string|null
    {
        # Create a new Model object for this value and validate it
        $model = new $this->model_class(data: $representation_value);
        return $model->to_internal();
    }

    /**
     * Converts the value from its internal value to its representation value.     
     * @param string $internal_value The internal value to be converted to its representation value
     */
    public function _from_internal(string $internal_value)
    {
        # Create a new Model object for this value and validate it
        $model = new $this->model_class();
        $model->from_internal
        return $model->to_internal();
    }
}
