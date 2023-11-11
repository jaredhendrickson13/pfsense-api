<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Fields\DateTimeField;

class APIFieldsDateTimeFieldTestCase extends TestCase
{
    /**
     * Checks that the is_valid_datetime() method correctly determines whether a datetime string matches a given format.
     */
    public function test_is_valid_datetime() {
        # Ensure valid datetimes return true
        $this->assert_is_true(DateTimeField::is_valid_datetime("12/31/1999", "m/d/Y"));
        $this->assert_is_true(DateTimeField::is_valid_datetime("Dec 31, 1999", "M d, Y"));

        # Ensure invalid datetimes return false
        $this->assert_is_false(DateTimeField::is_valid_datetime("13/31/1999", "m/d/Y"));
        $this->assert_is_false(DateTimeField::is_valid_datetime("Test 31, 1999", "M d, Y"));
        $this->assert_is_false(DateTimeField::is_valid_datetime("Dec 31, 1999", "m/d/Y"));
        $this->assert_is_false(DateTimeField::is_valid_datetime("12/31/1999", "M d, Y"));
    }
    
    /**
     * Checks that an error is thrown if this field is validated and its value does not match is datetime format
     */
    public function test_validate() {
        $this->assert_throws_response(
            response_id: "DATETIME_FIELD_MUST_MATCH_FORMAT",
            code: 400,
            callable: function () {
                $dt = new DateTimeField(required: true, datetime_format: "m/d/Y");
                $dt->name = "test_field";
                $dt->value = "not a datetime";
                $dt->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $dt = new DateTimeField(required: true, datetime_format: "m/d/Y");
                $dt->name = "test_field";
                $dt->value = "12/31/1999";
                $dt->validate();
            }
        );
    }
}
