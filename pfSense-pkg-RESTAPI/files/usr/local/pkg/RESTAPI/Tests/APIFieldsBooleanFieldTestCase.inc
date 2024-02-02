<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\BooleanField;

class APIFieldsBooleanFieldTestCase extends TestCase
{
    /**
     * Checks that the BooleanField's `from_internal()` method correctly represents the internal value.
     */
    public function test_boolean_field_from_internal()
    {
        # Create a BooleanField object that internally uses "on" to indicate `true` values and "off" for `false`
        $test_boolean_field = new BooleanField(default: false, indicates_true: 'on', indicates_false: 'off');

        # Ensure the internal values are correctly converted to their representation values
        $test_boolean_field->from_internal('on');
        $this->assert_is_true($test_boolean_field->value);
        $test_boolean_field->from_internal('off');
        $this->assert_is_false($test_boolean_field->value);
    }

    /**
     * Checks that the BooleanField's `to_internal()` method correctly converts representations to their internal value
     */
    public function test_boolean_field_to_internal()
    {
        # Create a BooleanField object that internally uses "on" to indicate `true` values and "off" for `false`
        $test_boolean_field = new BooleanField(default: false, indicates_true: 'on', indicates_false: 'off');

        # Ensure the representation values are correctly converted to their internal
        $test_boolean_field->value = true;
        $this->assert_equals($test_boolean_field->to_internal(), 'on');
        $test_boolean_field->value = false;
        $this->assert_equals($test_boolean_field->to_internal(), 'off');
    }
}
