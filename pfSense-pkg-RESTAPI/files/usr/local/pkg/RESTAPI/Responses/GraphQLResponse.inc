<?php

namespace RESTAPI\Responses;

require_once 'RESTAPI/autoloader.inc';

use GraphQL\Error\Error;
use GraphQL\Executor\ExecutionResult;
use RESTAPI\Core\Model;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\ResourceLinkSet;
use RESTAPI\Core\Response;
use RESTAPI\Models\GraphQL;

/**
 * Response class that represents a GraphQL response. Unlike other responses, this response does not represent a
 * successful or failed response, but rather a generic one where the error details are embedded in the response data.
 * This is essentially a wrapper around the ExecutionResult object from the GraphQL library.
 */
class GraphQLResponse extends Response {
    public $code = 200;
    public ExecutionResult $execution_result;

    /**
     * Constructs a new GraphQLResponse object.
     * @param mixed $message The verbose message to include in the Response. (unused)
     * @param string $response_id A unique response ID that can be used to identify this specific response. This should
     * be unique from any other Response object in the codebase! (unused)
     * @param Model|ModelSet $data The GraphQL Model object that contains the GraphQL ExecutionResult object.
     * @param ResourceLinkSet $_links A ResourceLinkSet containing applicable ResourceLinks for this response. (unused)
     */
    public function __construct(
        public $message = '',
        public string $response_id = '',
        public Model|ModelSet $data = new GraphQL(),
        public ResourceLinkSet $_links = new ResourceLinkSet(),
    ) {
        # Pull the ExecutionResult object from the GraphQL Model object if present, otherwise create a new one.
        $this->execution_result = $data->result ?? new ExecutionResult();
        parent::__construct($message, $response_id, $data, $_links);
    }

    /**
     * Converts this GraphQLResponse object to an array representation of its contents.
     * @return array An array containing the response details.
     */
    public function to_representation(): array {
        return $this->data->result->toArray();
    }

    /**
     * Converts a standard Response object into a GraphQLResponse object. This is useful for converting standard REST API
     * responses into responses that better adhere to the GraphQL specification.
     * @param Response $response The Response object to convert.
     * @return GraphQLResponse The converted GraphQLResponse object.
     */
    public static function to_graphql_response(Response $response): GraphQLResponse {
        # Variables
        $is_success = false;

        # Check if the Response object is a successful response.
        if ($response->code === 200) {
            $is_success = true;
        }

        # Create a new GraphQL Model object to store the ExecutionResult object.
        $graphql_model = new GraphQL();

        # Create an ExecutionResult object with the error details from the Response object.
        $graphql_model->result = new ExecutionResult(
            data: $is_success ? $response->data : null,
            errors: $is_success ? null : [new Error(message: $response->message, extensions: [$response->response_id])]
        );

        return new GraphQLResponse(data: $graphql_model);
    }
}
