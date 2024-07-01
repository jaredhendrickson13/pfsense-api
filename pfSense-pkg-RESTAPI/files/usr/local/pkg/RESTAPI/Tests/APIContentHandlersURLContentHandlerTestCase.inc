<?php

namespace RESTAPI\Tests;

use RESTAPI\ContentHandlers\URLContentHandler;
use RESTAPI\Core\TestCase;

class APIContentHandlersURLContentHandlerTestCase extends TestCase {
    /**
     * Checks that the `get_content()` method correctly obtains content from the PHP $_GET variable.
     */
    public function test_get_content(): void {
        # Create a handler to test with
        $handler = new URLContentHandler();

        # Ensure the value is an empty array if no data was provided
        $this->assert_equals($handler->get_content(), []);

        # Set the value to $_GET manually and ensure the handler picks it up
        $_GET = ['somedata' => 'somevalue'];
        $this->assert_equals($handler->get_content(), ['somedata' => 'somevalue']);
    }

    /**
     * Checks that the `decode()` method correctly decodes the incoming content.
     */
    public function test_decode(): void {
        # Create a handler to test with
        $handler = new URLContentHandler();

        # Ensure boolean strings are converted to booleans
        $this->assert_equals($handler->decode(['test' => 'true']), ['test' => true]);
        $this->assert_equals($handler->decode(['test' => 'false']), ['test' => false]);

        # Ensure numeric values are converted to integers
        $this->assert_equals($handler->decode(['test' => '0']), ['test' => 0]);
        $this->assert_equals($handler->decode(['test' => '25']), ['test' => 25]);
    }

    /**
     * Checks that the `can_encode()` returns false since this is not a valid response content-type
     */
    public function test_can_encode_returns_false(): void {
        # Create a handler to test with
        $handler = new URLContentHandler();
        $this->assert_is_false($handler->can_encode());
    }

    /**
     * Checks that the `can_decode()` returns true since this is a valid request content-type
     */
    public function test_can_decode_returns_true(): void {
        # Create a handler to test with
        $handler = new URLContentHandler();
        $this->assert_is_true($handler->can_decode());
    }
}
