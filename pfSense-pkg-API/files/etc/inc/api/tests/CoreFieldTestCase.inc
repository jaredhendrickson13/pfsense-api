<?php

namespace API\Tests;

use API;

class CoreFieldTestCase extends API\Core\TestCase
{
    /**
     * Ensures an error is thrown when a Field object has a `default` value that is not specified as a `choice`.
     */
    public function test_default_with_choices () {
        $this->assert_throws(
            exceptions: [API\Core\ServerError::class],
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

    public function test_context_required_with_unique() {
        # Ensure an error raised when a Field is `unique`, but no parent model context assigned during validation.
        $this->assert_throws(
            exceptions: [API\Core\ServerError::class],
            callable: function () {
                $test_field = new API\Core\Field(unique: true);
                $test_field->validate();
            }
        );
    }
}
