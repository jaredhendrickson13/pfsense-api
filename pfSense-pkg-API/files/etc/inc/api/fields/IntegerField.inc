<?php

namespace API\Fields;

require_once("api/core/Field.inc");

use API;

/**
 * Defines a StringField class that extends the core Field method. This Field adds validation and representation for
 * string Model fields.
 */
class IntegerField extends API\Core\Field {
    /**
     * Defines the IntegerField object and sets its options.
     * @param int $minimum The minimum value this value can be.
     * @param int $maximum The maximum value this value can be.
     * @param array $choices An array of explicit choices this string field value can be.
     * @param bool $unique If `true`, this field must be unique from all other parent model objects. Enabling this
     * option requires the Model $context to be set AND the Model $context must have a `config_path` set.
     * @param bool $allow_null If `true`, null values will be allowed by this field.
     * @param bool $required If `true`, this field is required to have a value at all times.
     * @param mixed|null $default Assign a default string value to assign this Field if no value is present.
     * @param bool $read_only If `true`, this field can only read its value and cannot write its value to config.
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
     * @param string $description Set a description for this field. This description will be used in API documentation.
     */
    public function __construct(
        int $minimum = 0,
        int $maximum = 99999999999,
        array $choices = [],
        bool $unique = false,
        bool $allow_null = false,
        bool $required = false,
        mixed $default = null,
        bool $read_only = false,
        bool $many = false,
        string $delimiter = ",",
        string $internal_name = "",
        string $internal_namespace = "",
        array $conditions = [],
        array $validators = [],
        string $description = "",
    )
    {
        parent::__construct(
            type: "integer",
            choices: $choices,
            unique: $unique,
            allow_empty: false,
            allow_null: $allow_null,
            required: $required,
            default: $default,
            read_only: $read_only,
            many: $many,
            delimiter: $delimiter,
            internal_name: $internal_name,
            internal_namespace: $internal_namespace,
            conditions: $conditions,
            validators: $validators + [new API\Validators\NumericRangeValidator(minimum: $minimum, maximum: $maximum)],
            description: $description
        );
    }

    /**
     * Converts the field value to its representation form from it's internal pfSense configuration value.
     * @param string $internal_value The internal value from the pfSense configuration.
     * @return int The field value in its representation form.
     */
    protected function _from_internal(string $internal_value) : int {
        # Throw an error if the value is not numeric
        if (!is_numeric($internal_value)) {
            throw new API\Core\ServerError(
                message: "Cannot parse IntegerField '$this->name' from internal because its internal value is not
                    a numeric value. Consider changing this field to a StringField.",
                ref_id: "INTEGER_FIELD_WITH_NON_INTEGER_INTERNAL_VALUE"
            );
        }
        return intval($internal_value);
    }
}
