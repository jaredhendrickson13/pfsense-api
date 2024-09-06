<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;
use RESTAPI\Core\Model;
use RESTAPI\Core\Response;
use RESTAPI\Models\GraphQL;
use RESTAPI\Responses\GraphQLResponse;

/**
 * Defines an Endpoint for the GraphQL API at /api/v2/graphql.
 */
class GraphQLEndpoint extends Endpoint {
    public Model|GraphQL $model;

    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/graphql';
        $this->model_name = 'GraphQL';
        $this->request_method_options = ['POST'];

        # Set help text for this Endpoint
        $this->post_help_text =
            'Execute a GraphQL query. For more information on utilizing the GraphQL API, please ' .
            'refer to https://pfrest.org/graphql.';

        # Construct the parent Endpoint object
        parent::__construct();
    }

    /**
     * A custom handler used to convert the incoming request data into a GraphQL response.
     * @param Response $response The incoming response object.
     * @return Response The response object to return to the client.
     */
    public function response_handler(Response $response): Response {
        # For non-successful responses, convert the error response into a GraphQL response.
        if ($response->code !== 200) {
            return GraphQLResponse::to_graphql_response($response);
        }

        # Otherwise, create a response using the GraphQL model's ExecutionResult object.
        return new GraphQLResponse(data: $this->model);
    }
}
