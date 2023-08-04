<?php

namespace API\Fields;

require_once("api/core/Field.inc");

use API;

/**
 * Defines a BooleanField class that extends the core Field method. This Field adds validation and representation for
 * boolean Model fields.
 */
class BooleanField extends API\Core\Field {
    public string $indicates_true;
    public string|null $indicates_false;

    /**
     * Defines the StringField object and sets its options.
     * @param string $indicates_true The internal string that indicates whether a value is `true`.
     * @param string|null $indicates_false The internal value that indicates whether a value is `false`. Use `null`
     * if the lack of a value for this field indicates `false` internally.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
     * $many is enabled.
     * @param bool $write_only Set the `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param string $internal_name Assign a different field name to use when referring to the internal field as it's
     * stored in the pfSense configuration.
     * @param string $internal_namespace Sets the namespace this field belongs to internally. This can be used to nest
     * the field internal value under a specific namespace as an associative array. This only applies to the internal
     * value, not the representation value.
     * @param array $conditions An array of conditions the field must meet to be included. This allows you to specify
     * conditions of other fields within the parent Model context. For example, if the parent Model context has two
     * fields, one field named `type` and the other being this field; and you only want this field to be included if
     * `type` is equal to `type1`, you could assign ["type" => "type1"] to this parameter.
     * @param array $validators An array of Validator objects to run against this field.
     * @param string $help_text Set a description for this field. This description will be used in API documentation.
     */
    public function __construct(
        bool $required = false,
        bool $unique = false,
        mixed $default = null,
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $read_only = false,
        bool $write_only = false,
        string $indicates_true = "",
        string $indicates_false = null,
        string $internal_name = "",
        string $internal_namespace = "",
        array $conditions = [],
        array $validators = [],
        string $help_text = ""
    )
    {
        $this->indicates_true = $indicates_true;
        $this->indicates_false = $indicates_false;
        parent::__construct(
            type: "boolean",
            required: $required,
            unique: $unique,
            default: $default,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            read_only: $read_only,
            write_only: $write_only,
            many: false,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace,
            conditions: $conditions,
            validators: $validators,
            help_text: $help_text
        );
    }

    /**
     * Converts the field value to its representation form from it's internal pfSense configuration value.
     * @param mixed $internal_value The internal value from the pfSense configuration.
     * @return bool The field value in its representation form.
     */
    protected function _from_internal(mixed $internal_value) : bool {
        # Check if the string matches our `indicates_true` value, if so, set this value as true.
        if ($internal_value === $this->indicates_true) {
            return true;
        }

        # Otherwise, we assume the value is `false`
        return false;
    }

    /**
     * Converts the field value to its internal form that is suitable for writing to the pfSense config.
     * @param mixed $representation_value The representation form of this field to convert to internal.
     * @return string|null The internal string used to indicate if the value is `true` or `false`.
     */
    protected function _to_internal(mixed $representation_value): string|null
    {
        # If the representation value is `true`, set it's internal value as the current `indicates_true` value.
        if ($representation_value === true) {
            return $this->indicates_true;
        }

        # Otherwise, just return the `indicates_false` value.
        return $this->indicates_false;
    }
}
