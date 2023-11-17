<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Fields\UnixTimeField;

class APIFieldsUnixTimeFieldTestCase extends TestCase
{
    /**
     * Checks that UnixTimeFields cannot have `auto_update_now` and `many` set at the same time.
     */
    public function test_no_many_with_auto_update_now() {
        $this->assert_throws_response(
            response_id: "UNIX_TIME_FIELD_CANNOT_AUTO_UPDATE_NOW_WITH_MANY",
            code: 500,
            callable: function () {
                new UnixTimeField(many: true, auto_update_now: true);
            }
        );
    }

    /**
     * Ensure the value is automatically set to the current datetime when `auto_add_now` is enabled.
     */
    public function test_auto_add_now_at_creation() {
        $dt = new UnixTimeField(auto_add_now: true);
        $current_time = time();
        $this->assert_equals($current_time, $dt->value);
    }

    /**
     * Ensure the value is automatically updated to the current time when `auto_update_now` is set
     */
    public function test_auto_update_now() {
        $dt = new UnixTimeField(auto_update_now: true);
        $current_time = time();
        $this->assert_equals($current_time, (int)$dt->to_internal());
    }
}
