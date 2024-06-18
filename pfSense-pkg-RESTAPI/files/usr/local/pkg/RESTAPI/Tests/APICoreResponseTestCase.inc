<?php

namespace RESTAPI\Tests;

use RESTAPI;

/**
 * Defines test case for testing RESTAPI\Core\Response.
 */
class APICoreResponseTestCase extends RESTAPI\Core\TestCase {
    /**
     * Checks that Response objects are throwable and checks that it corresponds with an HTTP 500 status code.
     */
    public function test_response_throwable() {
        $this->assert_throws_response(
            response_id: 'TEST_ERROR',
            code: 500,
            callable: function () {
                throw new RESTAPI\Core\Response(message: 'test error', response_id: 'TEST_ERROR');
            },
        );
    }

    /**
     * Checks that the `get_http_status_message()` method correctly returns the status message associated with a
     * given HTTP status code, or returns 'unknown' if the code is not a known HTTP status code.
     */
    public function test_get_http_status_message() {
        $this->assert_equals('unknown', RESTAPI\Core\Response::get_http_status_message(5000));

        foreach (RESTAPI\Core\HTTP_STATUS_MESSAGES as $code => $message) {
            $this->assert_equals(RESTAPI\Core\Response::get_http_status_message($code), $message);
        }
    }

    /**
     * Checks the Response object is correctly converted a representation form using `to_representation()`.
     */
    public function test_response_to_representation() {
        # Create FirewallAlias and Response objects to test with
        $alias = new RESTAPI\Models\FirewallAlias(data: ['name' => 'test', 'type' => 'host']);
        $response = new RESTAPI\Core\Response(message: 'Test message', response_id: 'TEST_RESPONSE_ID', data: $alias);

        # Ensure the response schema is expected
        $this->assert_equals($response->to_representation(), [
            'code' => 500,
            'status' => 'internal server error',
            'response_id' => 'TEST_RESPONSE_ID',
            'message' => 'Test message',
            'data' => $alias->to_representation(),
        ]);

        # Enable HATEOAS and ensure the response includes the _links field
        $api_settings = new RESTAPI\Models\RESTAPISettings();
        $api_settings->hateoas->value = true;
        $api_settings->update();

        # Ensure the response schema now includes the _links field
        $this->assert_equals($response->to_representation(), [
            'code' => 500,
            'status' => 'internal server error',
            'response_id' => 'TEST_RESPONSE_ID',
            'message' => 'Test message',
            'data' => $alias->to_representation(),
            '_links' => [],
        ]);
    }

    /**
     * Checks that the Response object is correctly converted to its OpenAPI component representation when the
     * `to_openapi_component()` method is run.
     */
    public function test_response_to_openapi_component() {
        # Create a Response object to represent as an OpenAPI component.
        $response = new RESTAPI\Core\Response(message: 'Test message', response_id: 'TEST_RESPONSE_ID');
        $response->help_text = 'Example OpenAPI description for this response component.';

        $this->assert_equals($response->to_openapi_component(), [
            'description' => 'Example OpenAPI description for this response component.',
            'content' => [
                'application/json' => [
                    'schema' => ['$ref' => '#/components/schemas/Response'],
                ],
            ],
        ]);
    }

    /**
     * Checks that the Response object is correctly converted it its OpenAPI schema representation when the
     * `to_openapi_schema()` method is run.
     */
    public function test_response_to_openapi_schema() {
        # Create a Response object to represent as an OpenAPI component.
        $response = new RESTAPI\Core\Response(message: 'Test message', response_id: 'TEST_RESPONSE_ID');
        $response->help_text = 'Example OpenAPI description for this response component.';

        $this->assert_equals($response->to_openapi_schema(), [
            'type' => 'object',
            'properties' => [
                'code' => [
                    'description' => 'The HTTP status code that corresponds with the API response.',
                    'type' => 'integer',
                ],
                'status' => [
                    'description' => 'The HTTP status message that corresponds with the HTTP status code.',
                    'type' => 'string',
                ],
                'response_id' => [
                    'description' =>
                        'The unique response ID that corresponds with the result of the API' .
                        'call. In most situations, this will contain an error code.',
                    'type' => 'string',
                ],
                'message' => [
                    'description' => 'The descriptive message detailing the results of the API call.',
                    'type' => 'string',
                ],
                'data' => [
                    'description' =>
                        'The data requested from the API. In the event that many objects have' .
                        'been requested, this field will be an array of objects. Otherwise, it will only return' .
                        'the single object requested.',
                    'example' => [],
                    'oneOf' => [['type' => 'array', 'items' => ['type' => 'object']], ['type' => 'object']],
                ],
                '_links' => [
                    'description' => 'An array of links to resources that are related to this API response.',
                    'type' => 'object',
                ],
            ],
        ]);
    }
}
