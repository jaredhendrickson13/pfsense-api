<?php

namespace RESTAPI\Tests;

use RESTAPI\ContentHandlers\JSONContentHandler;
use RESTAPI\Core\TestCase;

class APIContentHandlersJSONContentHandlerTestCase extends TestCase {
    /**
     * Checks that the `get_content()` method correctly obtains a client's unprocessed request body.
     */
    public function test_get_content(): void {
        # Create a handler to test with
        $handler = new JSONContentHandler();

        # Manually set php://input and ensure the handler obtains the set value exactly
        file_put_contents('php://input', 'test_value');
        $this->assert_equals($handler->get_content(), file_get_contents('php://input'));
    }

    /**
     * Checks that the `_encode()` method correctly converts a provide value into a JSON encoded string
     */
    public function test_encode(): void {
        # Create a handler to test with
        $handler = new JSONContentHandler();

        # Encode the given array into JSON using the encode() method
        $content = ['testkey' => 'testval'];
        $json_str = $handler->encode($content);

        # Decode the string from JSON and ensure it matches the original content
        $this->assert_equals(json_decode($json_str, associative: true), $content);
    }

    /**
     * Checks that the `_decode()` method correctly converts a provide value into a JSON decoded string
     */
    public function test_decode(): void {
        # Create a handler to test with
        $handler = new JSONContentHandler();

        # Decode the given string from JSON using the decode() method
        $content = '{"testkey":"testval"}';
        $json_str = $handler->decode($content);

        # Encode the string from JSON and ensure it matches the original content
        $this->assert_equals(json_encode($json_str), $content);
    }

    /**
     * Checks that JSONContentHandler's `can_encode()` and `can_decode()` both return true since these methods are
     * overridden in the child class.
     */
    public function test_can_encode_and_decode(): void {
        # Create a handler to test with
        $handler = new JSONContentHandler();
        $this->assert_is_true($handler->can_encode());
        $this->assert_is_true($handler->can_decode());
    }
}
