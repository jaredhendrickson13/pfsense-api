<?php

namespace API\Fields;

require_once("api/core/Field.inc");

use API;

/**
 * Defines a StringField class that extends the core Field method. This Field adds validation and representation for
 * string Model fields.
 */
class StringField extends API\Core\Field {
    /**
     * Defines the StringField object and sets its options.
     * @param int $min_length The minimum number of characters required for this string field.
     * @param int $max_length The maximum number of characters allowed by this string field.
     * @param array $choices An array of explicit choices this string field value can be.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
     * @param bool $write_only Set the `true` to make this field write-only. This will prevent the field's current value
     * from being displayed in the representation data. This is ideal for potentially sensitive fields like passwords,
     * keys, and hashes.
     * @param bool $many If `true`, the value must be an array of many strings.
     * @param string $delimiter Assign the delimiter used to join the value array back to its internal string form when
     * $many is enabled.
     * @param string $internal_name Assign a different field name to use when referring to the internal field as it's
     * stored in the pfSense configuration.
     * @param string $internal_namespace Sets the namespace this field belongs to internally. This can be used to nest
     * the fields internal value under a specific namespace as an associative array. This only applies to the internal
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
        array $choices = [],
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $read_only = false,
        bool $write_only = false,
        bool $many = false,
        int $many_minimum = 1,
        int $many_maximum = 128,
        int $min_length = 0,
        int $max_length = 1024,
        string $delimiter = ",",
        string $internal_name = "",
        string $internal_namespace = "",
        array $conditions = [],
        array $validators = [],
        string $help_text = ""
    )
    {
        parent::__construct(
            type: "string",
            required: $required,
            unique: $unique,
            default: $default,
            choices: $choices,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            read_only: $read_only,
            write_only: $write_only,
            many: $many,
            many_minimum: $many_minimum,
            many_maximum: $many_maximum,
            delimiter: $delimiter,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace,
            conditions: $conditions,
            validators: $validators + [new API\Validators\LengthValidator(minimum: $min_length, maximum: $max_length)],
            help_text: $help_text
        );
    }

    /**
     * Converts the field value to its representation form from it's internal pfSense configuration value.
     * @param mixed $internal_value The internal value from the pfSense configuration.
     * @return string The field value in its representation form.
     */
    protected function _from_internal(mixed $internal_value) : string {
        return strval($internal_value);
    }
}