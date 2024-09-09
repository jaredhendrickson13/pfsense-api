<?php

namespace RESTAPI\Tests;

use RESTAPI\ContentHandlers\BinaryContentHandler;
use RESTAPI\Core\TestCase;
use RESTAPI\Responses\Success;

class APIContentHandlersBinaryContentHandlerTestCase extends TestCase
{
    /**
     * Ensure the encode() method throws an error if the required 'binary_data' and 'filename' keys are not present in
     * the content array.
     */
    public function test_encode_requires_binary_data_and_filename_keys(): void
    {
        $this->assert_throws_response(
            response_id: 'BINARY_CONTENT_HANDLER_RESOURCE_NOT_SUPPORTED',
            code: 406,
            callable: function () {
                $handler = new BinaryContentHandler();
                $response = new Success(message: '', response_id: ''); # Required as context for the encode() method
                $handler->encode(['data' => []], $response);
            }
        );
    }

    /**
     * Ensure the encode() method throws an error if the 'filename' key contains invalid characters.
     */
    public function test_encode_requires_valid_filename(): void
    {
        $this->assert_throws_response(
            response_id: 'BINARY_CONTENT_HANDLER_INVALID_FILENAME',
            code: 500,
            callable: function () {
                $handler = new BinaryContentHandler();
                $response = new Success(message: '', response_id: ''); # Required as context for the encode() method
                $handler->encode(['data' => ['binary_data' => 'data', 'filename' => 'invalid file name']], $response);
            }
        );
    }

    /**
     * Ensure the encode() method does not throw an error if the required 'binary_data' and 'filename' keys are present in
     * the content array.
     */
    public function test_encode_requires_binary_data(): void {
        $this->assert_throws_response(
            response_id: 'BINARY_CONTENT_HANDLER_INVALID_DATA',
            code: 500,
            callable: function () {
                $handler = new BinaryContentHandler();
                $response = new Success(message: '', response_id: ''); # Required as context for the encode() method
                $handler->encode(
                    ['data' => ['binary_data' => 'data', 'filename' => 'valid_file_name.txt']],
                    $response
                );
            }
        );
    }

    /**
     * Ensures the encode() method correctly encodes the content as a binary file for download.
     */
    public function test_encode(): void
    {
        $this->assert_does_not_throw(
            callable: function () {
                $response = new Success(message: '', response_id: '');
                $handler = new BinaryContentHandler();
                $binary_data = random_bytes(16);
                $handler->encode(
                    ['data' => ['binary_data' => $binary_data, 'filename' => 'valid_file_name.txt']],
                    $response
                );
            }
        );
    }

    /**
     * Ensures the to_openapi_schema() method returns the correct schema for the BinaryContentHandler.
     */
    public function test_to_openapi_schema(): void
    {
        $handler = new BinaryContentHandler();
        $response = new Success(message: '', response_id: ''); # Required as context for the to_openapi_schema() method
        $this->assert_equals(
            $handler->to_openapi_schema($response),
            [
                $handler->mime_type => [
                    'description' => $handler->encode_help_text,
                    'schema' => [
                        'type' => 'string',
                        'format' => 'binary',
                    ],
                ],
            ]
        );
    }

}