<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Endpoints\GraphQLEndpoint;
use RESTAPI\Models\SystemHostname;
use RESTAPI\Responses\GraphQLResponse;
use RESTAPI\Responses\NotFoundError;
use RESTAPI\Responses\Success;
use RESTAPI\Responses\ValidationError;

class APIEndpointsGraphQLEndpointTestCase extends TestCase
{
    /**
     * Ensure the response_handler for the GraphQLEndpoint correctly converts the standard REST API response into a
     * GraphQL response.
     */
    public function test_response_handler(): void {
        # Create a few different Response objects to test with.
        $not_found_resp = new NotFoundError(message: 'Test not found', response_id: 'TEST_NOT_FOUND');
        $bad_request_resp = new ValidationError(message: 'Test bad request', response_id: 'TEST_BAD_REQUEST');
        $success_resp = new Success(message: 'Test success', response_id: 'TEST_SUCCESS');

        # Create a new GraphQLEndpoint object.
        $endpoint = new GraphQLEndpoint();

        # Ensure the response_handler correctly converts a Success response into a GraphQLResponse.
        $this->assert_is_true($endpoint->response_handler($not_found_resp) instanceof GraphQLResponse);
        $this->assert_is_true($endpoint->response_handler($bad_request_resp) instanceof GraphQLResponse);
        $this->assert_is_true($endpoint->response_handler($success_resp) instanceof GraphQLResponse);
    }

    /**
     * Make a POST request to /api/v2/graphql to ensure it works end-to-end.
     */
    public function test_endpoint_e2e(): void {

        $json_resp = \RESTAPI\Core\Tools\http_request(
            # Make a POST request to the GraphQL endpoint to read the system hostname.
            url: 'https://localhost/api/v2/graphql',
            method: 'POST',
            data: ['query' => "{ readSystemHostname { hostname domain } }"],
            headers: ['Content-Type' => 'application/json'],
            username: 'admin',
            password: 'pfsense',
            validate_certs: false,
        );
        $resp = json_decode($json_resp, associative: true);

        # Ensure the response is successful and contains the expected data.
        $hostname = new SystemHostname();
        $this->assert_equals(
            $resp,
            [
                'data' => [
                    'readSystemHostname' => [
                        'hostname' => $hostname->hostname->value,
                        'domain' => $hostname->domain->value,
                    ],
                ],
            ]
        );
    }
}