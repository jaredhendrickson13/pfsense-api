<?php

namespace API\Core;

class Field
{
    public mixed $value = null;
    public string $type = "";
    public array $validators = [];
    public bool $unique = false;
    public bool $many = false;
    public string $delimiter = ",";
    public array $choices = [];
    public bool $allow_empty = false;
    public bool $allow_null = false;
    public bool $required = false;
    public mixed $default = null;
    public bool $read_only = false;
    public string $description = "";
    public string $name = "";
    public string $internal_name = "";
    public array $when = [];
    public Model $context;

    /**
     * @param string $type The primary type supported by this field. This must be a string representation of the type
     * as it is returned by PHP's gettype() function.
     * @param array $validators An array of API\Core\Validator objects to use when validating this field.
     * @param array $choices An array of explicitly choices this field's value can be.
     * @param bool $unique Set to `true` if this value must be unique from all other objects of the same model.
     * @param bool $allow_empty Set to `true` if empty values (blank strings, 0, etc) should be allowed.
     * @param bool $allow_null Set to `true` if `null` values should be supported for this field. When `null` values
     * are supported, the underlying pfSense configuration field will be removed entirely when receiving a null value.
     * @param bool $required Set to `true` if this field is required to be explicitly set. $required and $default are
     * mutually exclusive.
     * @param mixed|null $default The default value to assign this field if no value was found. If not set, it assumed
     * this Field requires a value and should fail validation if no value was found.
     * @param bool $read_only Set to `true` to make this field read-only. Attempts to write to a read-only field will
     * result in a thrown exception.
     * @param bool $many Set to `true` if this field should accept an array of many values. `false` if only one value
     * should be allowed. If `true`, validation will run against each value in the value array individual.
     * @param string $delimiter Assigns the string delimiter to use when writing array values to config. This is only
     * available if $many is set to `true`. Defaults to `,` to store as comma-separated string.
     * @param string $description Sets a description for this field. This description will be used as the description
     * for this field in the API documentation.
     * @param string $internal_name The name of the field as it is stored in the pfSense configuration. If not set, the
     * internal name will utilize the $name of the Field object.
     */
    public function __construct(
        string $type = "",
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
    ){
        # Assign values to object properties
        $this->type = $type;
        $this->validators = $validators;
        $this->choices = $choices;
        $this->unique = $unique;
        $this->allow_empty = $allow_empty;
        $this->allow_null = ($read_only) ? true : $allow_null;
        $this->required = $required;
        $this->default = $default;
        $this->read_only = $read_only;
        $this->many = $many;
        $this->delimiter = $delimiter;
        $this->description = $description;

        # Ensure the $default value is in $choices if set
        if (isset($default) and $choices and !in_array($default, $choices, true)) {
            throw new ServerError(
                message: "Field object `default` must be one of available `choices`.",
                return: "INVALID_CHOICE_DEFAULT"
            );
        }

        # Only assign the internal_name if it is not empty.
        if ($internal_name) {
            $this->internal_name = $internal_name;
        }
    }


    /**
     * Runs all validations for this field.
     * @return bool `true` if all validation succeeded.
     * @throws ValidationError If any validations failed.
     * @throws ServerError If a programming error is detected.
     */
    public function validate() : bool {
        # Ensure model context exists if needed
        $this->check_model_context();

        # Ensure this field object has a name set
        $this->check_field_names();

        # Ensure a value is specified if field is `required`
        $this->check_field_required();

        # Ensure a value is not set if field is `read_only`
        $this->check_field_read_only();

        # Ensure the value(s) to validate are always in an array, so we can loop.
        $values = $this->get_value_as_array();

        # Loop through each value to be validated
        foreach($values as $value) {
            # Ensure the value's type is supported
            $this->check_value_type($value);

            # Ensure this value is an available choice (if `choices` are defined for this field)
            $this->check_value_choice($value);

            # Ensure this value is not empty unless empty values are allowed by this field
            $this->check_value_empty($value);

            # Ensure this value is not null unless null values are allowed by this field
            $this->check_value_null($value);

            # Loop through each validator and run it's `validate` method against the current value.
            foreach ($this->validators as $validator) {
                $validator->validate($value);
            }

            # Ensure value is unique if set
            $this->check_field_unique();
        }
        return true;
    }

    private function check_model_context() {
        # Throw an error if this field is unique but no model context was provided
        if ($this->unique and !$this->context) {
            throw new ServerError(
                message: "Unique fields must have model context provided.",
                return: "UNIQUE_FIELD_WITH_NO_CONTEXT"
            );
        }
    }

    /**
     * When the field is required, checks if a value is set and checks if mutually exclusive options are both set.
     * @throws ServerError When `required` and `default` are both set OR `required` and `allow_null` are both set.
     * @throws ValidationError If `required` is set but no value was assigned.
     */
    private function check_field_required() {
        # Throw an error if `required` and `default` are both set
        if ($this->required and isset($this->default)) {
            throw new ServerError(
                message: "Required field `$this->name` cannot have a default.",
                return: "CANNOT_REQUIRE_DEFAULT"
            );
        }

        # Throw an error if `required` and `allow_null` are both set
        if ($this->required and $this->allow_null) {
            throw new ServerError(
                message: "Required field `$this->name` cannot allow null values.",
                return: "CANNOT_REQUIRE_NULL"
            );
        }

        # Throw an error if this field is required but no value was set.
        if ($this->required and is_null($this->value)) {
            throw new ValidationError(
                message: "Field `$this->name` is required.",
                return: "MISSING_REQUIRED_FIELD"
            );
        }
    }

    /**
     * Ensures read-only values do not have a value, default or choices.
     * @throws ServerError When `read_only` is set and `choices` or `default` are also set.
     * @throws ValidationError When `read_only` is set and a non-null value was requested.
     */
    private function check_field_read_only() {
        # Throw an error if the field is read only, but a default value was set
        if ($this->read_only and isset($this->default)) {
            throw new ServerError(
                message: "Read-only field `$this->name` cannot have a default value.",
                return: "NO_DEFAULT_WITH_READ_ONLY"
            );
        }

        # Throw an error if the field is read only, but choices were defined
        if ($this->read_only and $this->choices) {
            throw new ServerError(
                message: "Read-only field `$this->name` cannot have value choices.",
                return: "NO_CHOICES_WITH_READ_ONLY"
            );
        }

        # Throw an error if this field has a non-null value and is a read-only field.
        if ($this->read_only and !is_null($this->value)) {
            throw new ValidationError(
                message: "Cannot assign value to read-only field `$this->name`.",
                return: "READ_ONLY_FIELD"
            );
        }
    }

    /**
     * Obtains an array of individual values for this field. If `many` is enabled, this method will ensure `value` is
     * already an array and simply return the array value. If `many` is not enabled, the individual `value` will be
     * wrapped in an array and returned. This intended to obtain ALL values that need to validated into
     * an array format for looping purposes.
     * @returns array An array containing all individual values that need validation.
     * @throws ValidationError When `many` is enabled, but `value` is not an array.
     */
    private function get_value_as_array() : array {
        # Assume the default value if no value was set and a default was specified
        if (isset($this->default) and !isset($this->value)) {
            $this->value = $this->default;
        }

        # Check if many is enabled
        if ($this->many) {
            # Throw an error if the value is not already an array
            if (!is_array($this->value)) {
                throw new ValidationError(
                    message: "Field `$this->name` must be an array.",
                    return: "FIELD_INVALID_MANY_VALUE"
                );
            }

            # Return the value array here if we didn't throw an error in the previous condition
            return $this->value;
        }

        # When many is not enabled, wrap the current `value` in an array so it can be used for looping.
        return [$this->value];
    }

    /**
     * Checks if this Field object has a valid `name` and `internal_name`. In the event that a `name` exists, but an
     * `internal_name` was not specified, the `internal_name` will be assigned the same value as `name` by this method.
     * @throws ServerError When this object's `name` has not been set.
     */
    public function check_field_names() {
        # Throw an error if no `name` has been set.
        if (empty($this->name)) {
            throw new ServerError(
                message: "Field object must be assigned a `name` value after it is constructed.",
                return: "FIELD_UNNAMED"
            );
        }

        # Default the `internal_name` to the field's `name` if it was not explicitly set.
        if (empty($this->internal_name)) {
            $this->internal_name = $this->name;
        }
    }

    /**
     * Checks if a 'unique' field is actually unique from all other objects related to the $context Model's config path.
     * The $context Model must have a `config_path` set AND have it's `many` value set to `true` to support unique
     * validation.
     * @throws ValidationError When the assigned $context Model does not have a `config_path` set OR the assigned
     * $context Model does not have it's $many value to `true`.
     */
    private function check_field_unique() {
        # Only check this if `unique` is enabled
        if ($this->unique) {
            # Throw an error if unique is set but the model context has no config path specified
            if (!$this->context->config_path) {
                throw new ServerError(
                    message: "Field '$this->name' cannot be 'unique' when its model context has no 'config_path'.",
                    return: "FIELD_UNIQUE_WITH_NO_MODEL_CONFIG_PATH"
                );
            }

            # Throw an error if unique is set but the model context is not a 'many' model.
            if (!$this->context->many) {
                throw new ServerError(
                    message: "Field '$this->name' cannot be 'unique' when its model context does not have 'many' enabled.",
                    return: "FIELD_UNIQUE_WITHOUT_MANY_MODEL_CONTEXT"
                );
            }

            # Fetch model objects for each internal object stored in config for this model
            $model_objects = $this->context->get_all_from_config();
            $field_name = $this->name;

            foreach ($model_objects as $model_object_id => $model_object) {
                # Move onto the next model object if this model object's field value does not match our objects value.
                if ($model_object->$field_name->value != $this->value) {
                    continue;
                }
                # Move onto the next item if the model object's ID matches our object's model context's ID.
                # This prevents `unique` errors on objects that are trying to update their own value with the value
                # it already has set.
                elseif ($model_object->id === $this->context->id) {
                    continue;
                }
                # When neither of previous conditions are met, we've found a unique conflict. Throw an error.
                else {
                    throw new ValidationError(
                        message: "Field '$this->name' must be unique. Value is in use by object with ID '$model_object_id'.",
                        return: "FIELD_MUST_BE_UNIQUE"
                    );
                }
            }
        }
    }

    /**
     * Checks if a given value's type matches this Field object's primary `type`
     * @param mixed $value The value to check against assigned `type`.
     * @throws ValidationError If the specified value is not a supported type.
     */
    private function check_value_type(mixed $value) {
        # Don't validate type for `read_only` fields
        if ($this->read_only) {
            return;
        }

        # Allow value to be NULL if `allow_null` is set
        if ($this->allow_null and gettype($value) === "NULL") {
            return;
        }

        # Raise an exception if the value is not the supported type.
        if ($this->type !== gettype($value)) {
            throw new ValidationError(
                message: "Field `$this->name` must be of type `$this->type`",
                return: "FIELD_INVALID_TYPE"
            );
        }
    }

    /**
     * Checks if a given `value` is a valid `choice` option when `choices` are specified.
     * @param mixed $value The value to check against available choices.
     * @returns ValidationError When the $value is not an available choice.
     */
    private function check_value_choice(mixed $value) {
        # If choices are set, ensure this value is one of the choices
        if ($this->choices and !in_array($value, $this->choices)) {
            # Convert the choices array to a string format and throw a choice not supported
            $choices_str = implode(", ", $this->choices);
            throw new ValidationError(
                message: "Field `$this->name` must be one of [$choices_str]",
                return: "FIELD_INVALID_CHOICE"
            );
        }
    }

    /**
     * Checks if a given `value` is an empty array or string and checks if empty values are allowed.
     * @param mixed $value The value to check for emptiness.
     * @returns ValidationError When the $value is not a supported type.
     */
    private function check_value_empty(mixed $value) {
        # Check if value is an empty string or array. These are the only two types we consider possible to be empty
        # to avoid having to enable `allow_empty` for booleans, 0 integers, and null values
        # (which are checked separately).
        if (!$this->allow_empty and ($value === "" or $value === [])) {
            throw new ValidationError(
                message: "Field `$this->name` cannot be empty.",
                return: "FIELD_EMPTY_NOT_ALLOWED"
            );
        }
    }

    /**
     * Checks if a given `value` is null and checks if null values are allowed.
     * @param mixed $value The value to check for emptiness.
     * @returns ValidationError When the $value is not a supported type.
     */
    private function check_value_null(mixed $value) {
        # Throw an error if the value is null and null values are not allowed.
        if (!$this->allow_null and is_null($value)) {
            throw new ValidationError(
                message: "Field `$this->name` cannot be nullified.",
                return: "FIELD_NULL_NOT_ALLOWED"
            );
        }
    }

    /**
     * Converts the stored internal config value to a represented value. This method must be overridden by a child
     * Field class with the necessary steps to convert the internal value to the representation value. This method must
     * return value in it's represented form! In the case of a `many` field, this method will be called for each
     * individual value after it's been converted to an array by from_internal(). You do not need to worry about
     * converting the $internal_value to an array in this method!
     * @param string $internal_value The internal config value to convert to a representation value.
     * @throws ServerError When the child Field class does not override this method.
     */
    protected function _from_internal(string $internal_value) : mixed {
        throw new ServerError(
            message: "Field '$this->name' does not have method of converting internal value to representation.",
            return: "FIELD_CANNOT_REPRESENT_INTERNAL_VALUE"
        );
    }

    /**
     * Sets a public version of _from_internal() that calls the Field's _from_internal() method. In the case
     * of a `many` Field, the internal value will be expanded into it's array form and _from_internal() will run
     * against each item within that array. This method will automatically assign the representation value to the
     * `value` property for this object.
     * @param string $internal_value The raw internal config value to convert to a representation value.
     */
    public function from_internal(string $internal_value) {
        # For `many` type fields, convert all values to representation.
        if ($this->many) {
            # Expand the value into it's array form
            $values = explode($this->delimiter, $internal_value);
            $representation = [];

            # Run the field's `to_representation()` method for value to represent
            foreach ($values as $value) {
                $representation[] = $this->_from_internal($value);
            }

            $this->value = $representation;
        }
        # Otherwise, just convert the value to representation directly
        else {
            $this->value = $this->_from_internal($internal_value);
        }
    }

    /**
     * Converts the represented value into the internal pfSense value. In most cases child Field classes do not need to
     * override this method as the default behavior of converting the value into a string is sufficient. However, this
     * method can be overridden by a child Field class with custom steps to convert the represented value back into
     * the internal value if needed. This method must return the value in its internal form! In the case of a `many`
     * field, this method will be called for each `$this->value` before it's been joined into it's internal string by
     * to_internal(). If `$this->value` is an array, you do not need to worry about joining `$this->value` into a
     * string. Leave it as an array.
     * @param mixed $representation_value The value to convert into it's internal form.
     * @return array|string The internal value(s) suitable for writing to the pfSense configuration.
     */
    protected function _to_internal(mixed $representation_value) : array|string {
        return strval($representation_value);
    }

    /**
     * Sets a public version of _to_internal() that calls the Field's _to_internal() method. In the case
     * of a `many` Field, _to_internal() method will run against all current values and will then be joined back into
     * the internal string value suitable for writing to the pfSense configuration.
     * @return array|string The internal array or string value suitable for writing the pfSense configuration.
     */
    public function to_internal() : array|string {
        # For `many` fields, allow for individual value conversions before joining array back into internal string value
        if ($this->many) {
            $internal_values = [];

            # Run the field's `to_representation()` method for value to represent
            foreach ($this->value as $value) {
                $internal_values[] = $this->_to_internal($value);
            }

            # Join the array back into it's internal string format.
            return implode($this->delimiter, $internal_values);
        }
        # Otherwise, just convert the individual value to it's internal value
        else {
            return $this->_to_internal($this->value);
        }
    }
}
