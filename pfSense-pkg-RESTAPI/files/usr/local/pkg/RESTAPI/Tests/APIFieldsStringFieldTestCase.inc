<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\StringField;

class APIFieldsStringFieldTestCase extends TestCase {
    /**
     * Checks to ensure StringField value's internal values are always strings
     */
    public function test_string_field_from_internal() {
        # Create an example string field and ensure its value is always a string
        $test_string_field = new StringField(default: '');
        $test_string_field->from_internal('test string');
        $this->assert_equals($test_string_field->value, 'test string');
    }

    /**
     * Checks that the LengthValidator is always present for string Fields.
     */
    public function test_string_field_length_validator_default() {
        # Create an example string field and ensure it is created with a LengthValidator
        $test_string_field = new StringField();
        $this->assert_equals(get_class($test_string_field->validators[0]), 'RESTAPI\Validators\LengthValidator');
    }
}
