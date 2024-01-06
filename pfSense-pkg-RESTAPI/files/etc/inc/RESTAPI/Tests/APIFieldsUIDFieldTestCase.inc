<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\UIDField;

class APIFieldsUIDFieldTestCase extends TestCase
{
    /**
     * Checks that the UIDField defaults to a uniqid
     */
    public function test_default(): void {
        $field1 = new UIDField();
        $field2 = new UIDField();
        $this->assert_not_equals($field1->default, $field2->default);
    }
    
    /**
     * Checks that the 'prefix' attributes works as intended.
     */
    public function test_prefix(): void {
        $field = new UIDField(prefix: "test-");
        $this->assert_is_true(str_starts_with($field->default, "test-"));
    }
}
