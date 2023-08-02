<?php

namespace API\Fields;

require_once("api/core/Field.inc");

use API;

/**
 * Defines a StringField class that extends the core Field method. This Field adds validation and representation for
 * string Model fields.
 */
class StringField extends API\Core\Field {
    public int $max_length;

    /**
     * Defines the StringField object and sets its options.
     * @param int $max_length The maximum number of characters allowed by this string field.
     * @param array $validators An array of Validator objects to run against this field.
     * @param array $choices An array of explicit choices this string field value can be.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param bool $allow_empty If `true`, empty strings will be allowed by this field.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
     * @param bool $many If `true`, the value must be an array of many strings.
     * @param string $delimiter Assign the delimiter used to join the value array back to its internal string form when
     * $many is enabled.
     * @param string $description Set a description for this field. This description will be used in API documentation.
     * @param string $internal_name Assign a different field name to use when referring to the internal field as it's
     * stored in the pfSense configuration.
     * @param string $internal_namespace Sets the namespace this field belongs to internally. This can be used to nest
     * the fields internal value under a specific namespace as an associative array. This only applies to the internal
     * value, not the representation value.
     */
    public function __construct(
        int $max_length = 128,
        array $validators = [],
        array $choices = [],
        bool $unique = false,
        bool $allow_empty = false,
        bool $allow_null = false,
        bool $required = false,
        mixed $default = null,
        bool $read_only = false,
        bool $many = false,
        string $delimiter = ",",
        string $description = "",
        string $internal_name = "",
        string $internal_namespace = ""
    )
    {
        parent::__construct(
            type: "string",
            validators: $validators + [new API\Validators\MaxLengthValidator($max_length)],
            choices: $choices,
            unique: $unique,
            allow_empty: $allow_empty,
            allow_null: $allow_null,
            required: $required,
            default: $default,
            read_only: $read_only,
            many: $many,
            delimiter: $delimiter,
            description: $description,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace
        );
        $this->max_length = $max_length;
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
