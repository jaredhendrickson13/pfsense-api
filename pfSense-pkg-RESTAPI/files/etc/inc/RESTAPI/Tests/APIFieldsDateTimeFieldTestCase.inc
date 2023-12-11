<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\DateTimeField;
use DateTime;

class APIFieldsDateTimeFieldTestCase extends TestCase
{
    /**
     * Checks that DateTimeFields cannot have `auto_update_now` and `many` set at the same time.
     */
    public function test_no_many_with_auto_update_now() {
        $this->assert_throws_response(
            response_id: "DATETIME_FIELD_CANNOT_AUTO_UPDATE_NOW_WITH_MANY",
            code: 500,
            callable: function () {
                new DateTimeField(auto_update_now: true, many: true);
            }
        );
    }

    /**
     * Ensure the value is automatically set to the current datetime when `auto_add_now` is enabled.
     */
    public function test_auto_add_now_at_creation() {
        $dt = new DateTimeField(auto_add_now: true);
        $current_datetime = (new DateTime())->format($dt->datetime_format);
        $this->assert_equals($current_datetime, $dt->value);
    }

    /**
     * Ensure the value is automatically updated to the current time when `auto_update_now` is set
     */
    public function test_auto_update_now() {
        $dt = new DateTimeField(auto_update_now: true, datetime_format: "m/d/Y H:i:s");
        $current_datetime = (new DateTime())->format($dt->datetime_format);
        $this->assert_equals($current_datetime, $dt->to_internal());
    }
    
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
