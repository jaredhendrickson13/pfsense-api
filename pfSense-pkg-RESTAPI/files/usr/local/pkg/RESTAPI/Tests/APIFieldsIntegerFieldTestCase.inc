<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\IntegerField;

class APIFieldsIntegerFieldTestCase extends TestCase
{
    /**
     * Checks to ensure IntegerField value's internal values are always integers
     */
    public function test_integer_field_from_internal()
    {
        # Create an example integer field and ensure its value is always an integer
        $test_string_field = new IntegerField();
        $test_string_field->from_internal('2000');
        $this->assert_equals($test_string_field->value, 2000);
    }

    /**
     * Checks that the NumericRangeValidator is always present for integer Fields.
     */
    public function test_integer_field_length_validator_default()
    {
        # Create an example integer field and ensure it is created with a NumericRangeValidator
        $test_string_field = new IntegerField();
        $this->assert_equals(get_class($test_string_field->validators[0]), 'RESTAPI\Validators\NumericRangeValidator');
    }
}
