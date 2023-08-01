<?php

namespace API\Tests;

use API;

/**
 * Defines test case for testing API\Core\Field.
 */
class APICoreFieldTestCase extends API\Core\TestCase
{
    /**
     * Ensures an error is thrown when a Field object has a `default` value that is not specified as a `choice`.
     */
    public function test_default_with_choices () {
        $this->assert_throws_response(
            ref_id: "FIELD_INVALID_CHOICE_DEFAULT",
            code: 500,
            callable: function () {
                new API\Core\Field(choices: ["a", "b"], default: "INVALID_CHOICE");
            }
        );
    }

    /**
     * Ensures Field properties are properly set when the Field object is created.
     */
    public function test_field_construction() {
        # Values
        $type = "string";
        $validators = [new API\Core\Validator()];
        $choices = ["a", "b"];
        $unique = true;
        $allow_empty = true;
        $allow_null = true;
        $required = true;
        $default = "a";
        $read_only = true;
        $many = true;
        $delimiter = " ";
        $description = "Test description";
        $internal_name = "test_name";

        # Create a Field object and ensure all values were properly set during construction
        $test_field = new API\Core\Field(
            type: $type,
            validators: $validators,
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
            internal_name: $internal_name
        );

        # Ensure the values we passed in are the values that are assigned to the object
        $this->assert_equals($test_field->type, $type);
        $this->assert_equals($test_field->validators, $validators);
        $this->assert_equals($test_field->choices, $choices);
        $this->assert_equals($test_field->unique, $unique);
        $this->assert_equals($test_field->allow_empty, $allow_empty);
        $this->assert_equals($test_field->allow_null, $allow_null);
        $this->assert_equals($test_field->required, $required);
        $this->assert_equals($test_field->default, $default);
        $this->assert_equals($test_field->read_only, $read_only);
        $this->assert_equals($test_field->many, $many);
        $this->assert_equals($test_field->delimiter, $delimiter);
        $this->assert_equals($test_field->description, $description);
        $this->assert_equals($test_field->internal_name, $internal_name);
    }

    /**
     * Checks that a unique Field is required to have a parent Model context assigned.
     */
    public function test_context_required_with_unique() {
        # Ensure an error raised when a Field is `unique`, but no parent model context assigned during validation.
        $this->assert_throws_response(
            ref_id: "FIELD_UNIQUE_WITH_NO_CONTEXT",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(unique: true);
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that Fields must have names assigned.
     */
    public function test_field_requires_name() {
        # Ensure the field has a name when it's validated.
        $this->assert_throws_response(
            ref_id: "FIELD_UNNAMED",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(required: true);
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that a Field's `internal_name` defaults to the Field's `name` if it's not set.
     */
    public function test_field_name_sets_internal_name_default() {
        # Ensure the fields `internal_name` defaults the `name` if it's not explicitly set.
        $test_field = new API\Core\Field(type: "string", default: "test");
        $test_field->name = "test_field";
        $test_field->validate();
        $this->assert_equals($test_field->name, $test_field->internal_name);
    }

    /**
     * Checks that a Field cannot contain a `default` value if the Field is `required`.
     */
    public function test_no_default_when_field_is_required() {
        # Ensure Fields cannot be required if they have a default assigned
        $this->assert_throws_response(
            ref_id: "FIELD_CANNOT_BE_REQUIRED_WITH_DEFAULT",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(required: true, default: "");
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that a Field cannot `allow_null` if the Field is `required`.
     */
    public function test_no_allow_null_when_field_is_required()
    {
        # Ensure Fields cannot be required if they allow null
        $this->assert_throws_response(
            ref_id: "FIELD_CANNOT_BE_REQUIRED_WITH_ALLOW_NULL",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(allow_null: true, required: true);
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that a Field actually requires a value when the Field is `required`.
     */
    public function test_require_value_when_field_is_required() {
        # Ensure required fields must have a value set
        $this->assert_throws_response(
            ref_id: "FIELD_IS_REQUIRED",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(required: true);
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that a Field that is not required must have a default value.
     */
    public function test_require_default_when_field_is_not_required() {
        # Ensure required fields must have a value set
        $this->assert_throws_response(
            ref_id: "FIELD_NEEDS_DEFAULT",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(required: false);
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that the field cannot contain a `default` value when it is `read_only`.
     */
    public function test_no_default_when_field_is_read_only() {
        $this->assert_throws_response(
            ref_id: "FIELD_NO_DEFAULT_WITH_READ_ONLY",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(type: "string", default: "test", read_only: true);
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that the field cannot contain `choices` when it is `read_only`.
     */
    public function test_no_choices_when_field_is_read_only() {
        $this->assert_throws_response(
            ref_id: "FIELD_NO_CHOICES_WITH_READ_ONLY",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(type: "string", choices: ["test"], read_only: true);
                $test_field->name = "test_field";
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that the field's parent model context has a config path set if the field is `unique`.
     */
    public function test_context_has_config_path_when_field_is_unique() {
        $this->assert_throws_response(
            ref_id: "FIELD_UNIQUE_WITH_NO_MODEL_CONFIG_PATH",
            code: 500,
            callable: function () {
                # Create a model to use for testing
                $test_model = new API\Core\Model();

                # Create a field to test with and ensure the test model is assigned as its parent context
                $test_field = new API\Core\Field(type: "string", unique: true, required: true);
                $test_field->name = "test_field";
                $test_field->value = "test_value";
                $test_field->context = $test_model;
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that the field's parent model context is a `many` model if the field is `unique`.
     */
    public function test_context_has_many_when_field_is_unique() {
        $this->assert_throws_response(
            ref_id: "FIELD_UNIQUE_WITHOUT_MANY_MODEL_CONTEXT",
            code: 500,
            callable: function () {
                # Create a model to use for testing
                $test_model = new API\Core\Model();
                $test_model->config_path = "test/config/path";

                # Create a field to test with and ensure the test model is assigned as its parent context
                $test_field = new API\Core\Field(type: "string", unique: true, required: true);
                $test_field->name = "test_field";
                $test_field->value = "test_value";
                $test_field->context = $test_model;
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that an error is thrown if a value is set with a type that doesn't match this field.
     */
    public function test_field_type_constraint() {
        $this->assert_throws_response(
            ref_id: "FIELD_INVALID_TYPE",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(type: "string", required: true);
                $test_field->name = "test_field";
                $test_field->value = 1;
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that null values are not allowed unless `allow_null` is set.
     */
    public function test_no_empty_values_without_allow_empty() {
        # Check for string
        $this->assert_throws_response(
            ref_id: "FIELD_EMPTY_NOT_ALLOWED",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(type: "string", required: true);
                $test_field->name = "test_field";
                $test_field->value = "";
                $test_field->validate();
            }
        );

        # Check an array (field has `many` set).
        $this->assert_throws_response(
            ref_id: "FIELD_EMPTY_NOT_ALLOWED",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(type: "string", required: true, many: true);
                $test_field->name = "test_field";
                $test_field->value = [];
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that assigned Validators are run against the Field value during validation.
     */
    function test_validators_check_values() {
        # Check against single value
        $this->assert_throws_response(
            ref_id: "EXCEEDS_MAX_LENGTH",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(
                    type: "string",
                    validators: [
                        new API\Validators\MaxLengthValidator(max_length: 5)
                    ],
                    required: true
                );
                $test_field->name = "test_field";
                $test_field->value = "THIS_VALUE_EXCEEDS_OUR_MAX_LENGTH";
                $test_field->validate();
            }
        );

        # Check against many value
        $this->assert_throws_response(
            ref_id: "EXCEEDS_MAX_LENGTH",
            code: 400,
            callable: function () {
                $test_field = new API\Core\Field(
                    type: "string",
                    validators: [
                        new API\Validators\MaxLengthValidator(max_length: 5)
                    ],
                    required: true,
                    many: true
                );
                $test_field->name = "test_field";
                $test_field->value = ["OK", "THIS_VALUE_EXCEEDS_OUR_MAX_LENGTH"];
                $test_field->validate();
            }
        );
    }

    /**
     * Checks that the default to_internal() methods converts values to proper types AND joins `many` enabled fields
     * back into strings using the specified `delimiter`.
     */
    public function test_default_to_internal_conversion() {
        # Check for single values
        $test_field = new API\Core\Field(type: "integer", required: true);
        $test_field->name = "test_field";
        $test_field->value = 0;
        $this->assert_equals($test_field->to_internal(), "0");

        # Check for many values
        $test_field = new API\Core\Field(type: "integer", required: true, many: true, delimiter: " ");
        $test_field->name = "test_field";
        $test_field->value = [0, 1, 2, 3, 4, 5];

        # Ensure the internal value of `many` fields is joined back into a string using the `delimiter`
        $this->assert_equals($test_field->to_internal(), implode($test_field->delimiter, $test_field->value));

        # Loop through each set value and ensure it is a string
        foreach ($test_field->to_internal() as $internal_value) {
            $this->assert_type($internal_value, "string");
        }
    }

    /**
     * Checks that the default from_internal() method throws an error for not being able to represent the field.
     */
    public function test_default_from_internal_conversion() {
        $this->assert_throws_response(
            ref_id: "FIELD_CANNOT_REPRESENT_INTERNAL_VALUE",
            code: 500,
            callable: function () {
                $test_field = new API\Core\Field(type: "integer", required: true);
                $test_field->name = "test_field";
                $test_field->from_internal("test");
            }
        );
    }
}
