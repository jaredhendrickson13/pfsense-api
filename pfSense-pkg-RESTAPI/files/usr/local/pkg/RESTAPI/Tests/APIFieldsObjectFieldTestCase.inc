<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\ObjectField;

class APIFieldsObjectFieldTestCase extends TestCase
{
    /**
     * Ensure that the 'validate_extra()' method does not allow the ObjctField's value to be anything but an
     * associative array
     */
    public function test_validate_extra(): void {
        # Variables
        $field = new ObjectField();

        # Ensure bad values throw an exception
        $this->assert_throws_response(
            response_id: "OBJECT_FIELD_INVALID_VALUE",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra([0,1,2]);
            }
        );

        # Ensure good values do not throw an exception
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra(["good" => "value"]);
                $field->validate_extra(['another' => ['good' => 'value']]);
            }
        );
    }

    /**
     * Ensure that the from_internal() method simply returns the array value as is
     */
    public function test_from_internal(): void
    {
        # Variables
        $field = new ObjectField();
        $field->from_internal(['good' => ['value']]);

        # Ensure the value is returned as is
        $this->assert_equals(
            ['good' => ['value']],
            $field->value
        );
    }
}