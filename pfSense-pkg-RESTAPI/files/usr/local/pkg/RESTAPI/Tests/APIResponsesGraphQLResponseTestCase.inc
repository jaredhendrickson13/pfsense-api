<?php

namespace RESTAPI\Tests;

use GraphQL\Executor\ExecutionResult;
use RESTAPI\Core\Auth;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\GraphQL;
use RESTAPI\Models\SystemHostname;
use RESTAPI\Responses\GraphQLResponse;
use RESTAPI\Responses\ValidationError;

class APIResponsesGraphQLResponseTestCase extends TestCase {
    /**
     * @var Auth $auth;
     * Auth object to pass to GraphQL for privilege checks.
     */
    public Auth $auth;

    /**
     * Ensures we have a valid Auth object to pass to GraphQL for privilege checks.
     */
    public function setup(): void {
        $this->auth = new Auth();
        $this->auth->username = 'admin';
    }

    /**
     * Ensure an execution result is always present when constructing a GraphQLResponse object.
     */
    public function test_execution_result(): void {
        # Create a new GraphQLResponse object.
        $response = new GraphQLResponse();

        # Ensure the ExecutionResult object is always present.
        $this->assert_is_true($response->execution_result instanceof ExecutionResult);
    }

    /**
     * Ensure the GraphQLResponse object is correctly represented as an array when calling to_representation.
     */
    public function test_to_representation(): void {
        # Test success response
        $hostname = new SystemHostname();
        $graphql = new GraphQL(query: '{ readSystemHostname { hostname domain } }', client: $this->auth);
        $graphql->create();
        $graphql_resp = new GraphQLResponse(data: $graphql);
        $this->assert_equals($graphql_resp->to_representation(), [
            'data' => [
                'readSystemHostname' => [
                    'hostname' => $hostname->hostname->value,
                    'domain' => $hostname->domain->value,
                ],
            ],
        ]);

        # Ensure any errors that occur are also represented correctly.
        $graphql = new GraphQL(query: '{ readFirewallRule(id: 65535) { id } }', client: $this->auth);
        $graphql->create();
        $graphql_resp = new GraphQLResponse(data: $graphql);
        $this->assert_equals($graphql_resp->to_representation(), [
            'errors' => [
                [
                    'message' => 'Object with ID `65535` does not exist.',
                    'locations' => [['line' => 1, 'column' => 3]],
                    'path' => ['readFirewallRule'],
                    'extensions' => ['response_id' => 'MODEL_OBJECT_NOT_FOUND'],
                ],
            ],
            'data' => ['readFirewallRule' => null],
        ]);
    }

    /**
     * Ensure the to_graphql_response method correctly converts a standard Response object into a GraphQLResponse object.
     */
    public function test_to_graphql_response(): void {
        # Create a few different Response objects to test with.
        $not_found_resp = new ValidationError(message: 'Test not found', response_id: 'TEST_NOT_FOUND');
        $bad_request_resp = new ValidationError(message: 'Test bad request', response_id: 'TEST_BAD_REQUEST');
        $success_resp = new ValidationError(message: 'Test success', response_id: 'TEST_SUCCESS');

        # Ensure the to_graphql_response method correctly converts a standard Response object into a GraphQLResponse object.
        $this->assert_is_true(GraphQLResponse::to_graphql_response($not_found_resp) instanceof GraphQLResponse);
        $this->assert_is_true(GraphQLResponse::to_graphql_response($bad_request_resp) instanceof GraphQLResponse);
        $this->assert_is_true(GraphQLResponse::to_graphql_response($success_resp) instanceof GraphQLResponse);

        # Ensure the error message is correctly set in the GraphQLResponse object.
        $graphql_resp = GraphQLResponse::to_graphql_response($not_found_resp);
        $this->assert_equals($graphql_resp->to_representation(), [
            'errors' => [
                [
                    'message' => 'Test not found',
                    'extensions' => ['response_id' => 'TEST_NOT_FOUND'],
                ],
            ],
        ]);
    }

    /**
     * Ensure the to_openapi_schema method correctly returns the OpenAPI schema for a GraphQLResponse object.
     */
    public function test_to_openapi_schema(): void {
        # Create a new GraphQLResponse object.
        $response = new GraphQLResponse();

        # Ensure the OpenAPI schema is correctly returned.
        $this->assert_equals($response->to_openapi_schema(), [
            'type' => 'object',
            'properties' => [
                'data' => ['description' => 'The GraphQL response data.', 'type' => 'object'],
                'errors' => [
                    'description' => 'The GraphQL response errors.',
                    'type' => 'array',
                    'items' => [
                        'type' => 'object',
                        'properties' => [
                            'message' => [
                                'description' => 'The error message.',
                                'type' => 'string',
                            ],
                            'extensions' => [
                                'description' => 'The error extensions.',
                                'type' => 'object',
                                'properties' => [
                                    'response_id' => [
                                        'description' => 'The error response ID.',
                                        'type' => 'string',
                                    ],
                                ],
                            ],
                            'locations' => [
                                'description' => 'The error locations.',
                                'type' => 'array',
                                'items' => [
                                    'type' => 'object',
                                    'properties' => [
                                        'line' => [
                                            'description' => 'The error line.',
                                            'type' => 'integer',
                                        ],
                                        'column' => [
                                            'description' => 'The error column.',
                                            'type' => 'integer',
                                        ],
                                    ],
                                ],
                            ],
                            'path' => [
                                'description' => 'The error path.',
                                'type' => 'array',
                                'items' => [
                                    'type' => 'string',
                                ],
                            ],
                        ],
                    ],
                ],
            ],
        ]);
    }
}
