<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\ContentHandler;
use RESTAPI\Core\TestCase;

class APICoreContentHandlerTestCase extends TestCase
{
    /**
     * Checks that attempts to run the `get_content()` method throw a server error if its not overridden by a child
     * class.
     */
    public function test_get_content_throws_error_if_not_overridden(): void {
        $this->assert_throws_response(
            response_id: "CONTENT_HANDLER_GET_CONTENT_NOT_OVERRIDDEN",
            code: 500,
            callable: function () {
                $handler = new ContentHandler();
                $handler->get_content();
            }
        );
    }
    
    /**
     * Checks that attempts to run the `encode()` method when the protected `_encode()` method has not been overridden
     * by a child class results in an error being thrown.
     */
    public function test_encode_throws_error_if_not_overridden(): void {
        $this->assert_throws_response(
            response_id: "CONTENT_HANDLER_CANNOT_ENCODE_CONTENT",
            code: 406,
            callable: function () {
                # The `_encode()` method is protected, but we can trigger the error by calling `encode()`
                $handler = new ContentHandler();
                $handler->mime_type = "test";
                $handler->encode(content: null);
            }
        );
    }

    /**
     * Checks that attempts to run the `decode()` method when the protected `_decode()` method has not been overridden
     * by a child class results in an error being thrown.
     */
    public function test_decode_throws_error_if_not_overridden(): void {
        $this->assert_throws_response(
            response_id: "CONTENT_HANDLER_CANNOT_DECODE_CONTENT",
            code: 415,
            callable: function () {
                # The `_decode()` method is protected, but we can trigger the error by calling `decode()`
                $handler = new ContentHandler();
                $handler->mime_type = "test";
                $handler->decode(content: "test");
            }
        );
    }
    
    /**
     * Checks that the `can_encode()` method always returns true for the core ContentHandler since it has
     * set the _encode() method within its own class.
     */
    public function test_can_encode(): void {
        $handler = new ContentHandler();
        $this->assert_is_true($handler->can_encode());
    }

    /**
     * Checks that the `can_decode()` method always returns true for the core ContentHandler since it has
     * set the _decode() method within its own class.
     */
    public function test_can_decode(): void {
        $handler = new ContentHandler();
        $this->assert_is_true($handler->can_decode());
    }
    
}
