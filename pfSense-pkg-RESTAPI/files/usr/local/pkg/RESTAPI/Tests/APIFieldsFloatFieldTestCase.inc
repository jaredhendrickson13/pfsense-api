<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\FloatField;
use RESTAPI\Validators\NumericRangeValidator;

class APIFieldsFloatFieldTestCase extends TestCase {
    /**
     * Checks to ensure FloatField value's internal values are always floats
     */
    public function test_float_field_from_internal(): void {
        # Create an example float field and ensure its value is always an float
        $test_string_field = new FloatField();
        $test_string_field->from_internal('20.15');
        $this->assert_equals($test_string_field->value, 20.15);
        $test_string_field->from_internal('0.15144352');
        $this->assert_equals($test_string_field->value, 0.15144352);
        $test_string_field->from_internal('50');
        $this->assert_equals($test_string_field->value, 50.0);
    }

    /**
     * Checks to ensure FloatField value's internal values can be comma separated decimals
     */
    public function test_float_field_from_internal_comma_separated(): void
    {
        # Create an example float field and ensure its value is always an float
        $test_string_field = new FloatField();
        $test_string_field->from_internal('20,15');
        $this->assert_equals($test_string_field->value, 20.15);
        $test_string_field->from_internal('0,15144352');
        $this->assert_equals($test_string_field->value, 0.15144352);
        $test_string_field->from_internal('50');
        $this->assert_equals($test_string_field->value, 50.0);
    }

    /**
     * Checks that the NumericRangeValidator is always present for float Fields.
     */
    public function test_float_field_length_validator_default(): void {
        # Create an example float field and ensure it is created with a NumericRangeValidator
        $test_string_field = new FloatField();
        $this->assert_is_true($test_string_field->validators[0] instanceof NumericRangeValidator);
    }
}
