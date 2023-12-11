<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\Base64Field;

class APIFieldsBase64FieldTestCase extends TestCase
{
    /**
     * Checks that Base64Field values are Base64 encoded when converted to their internal values.
     */   
    public function test_to_internal() {
        $b64 = new Base64Field();
        $b64->value = "test";
        $this->assert_equals($b64->to_internal(), base64_encode("test"));
    }

    /**
     * Checks that Base64Field values are Base64 decoded when converted from their internal values.
     */
    public function test_from_internal() {
        # Determine the base64 encoded value
        $b64_encoded_val = base64_encode("test");
        
        $b64 = new Base64Field();
        $b64->from_internal($b64_encoded_val);
        $this->assert_equals($b64->value, "test");
    }
}
