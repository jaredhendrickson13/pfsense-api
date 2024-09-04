<?php

namespace RESTAPI\GraphQL;

require_once 'RESTAPI/autoloader.inc';

use GraphQL\Error\Error;
use RESTAPI\Core\Model;
use RESTAPI\Core\Response;

/**
 * Defines a class that maps/resolves GraphQL queries and mutations to the appropriate Model methods.
 */
class Resolver
{
    /**
     * Constructs the GraphQLResolver object with a given Model object.
     * @param Model $model The Model object to be used for resolving queries and mutations.
     */
    public function __construct(public Model $model) {
    }

    /**
     * A resolver that maps a GraphQL query to the Model's 'query' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function query($root, $args, $context, $info): array {
        try {
            # Execute the query
            $query = $this->model->query(
                query_params: $args['query_params'],
                limit: $args['limit'],
                offset: $args['offset'],
                reverse: $args['reverse'],
            );

            # Return the query results in an array representation
            return $query->to_representation();
        }
        catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }

    }

    /**
     * A resolver that maps a GraphQL query to the Model's 'read' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function read($root, $args, $context, $info): array
    {
        try {
            # Read the requested object using the Model's 'read' method (which is called on construction)
            $model = new $this->model(id: $args['id'] ?? null, parent_id: $args['parent_id'] ?? null);

            # Return the read results in an array representation
            return $model->to_representation();
        }
        catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'create' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function create($root, $args, $context, $info): array {
        try {
            # Create a new Model object with the given data
            $model = new $this->model(data: $args);

            # Return the created object in an array representation
            return $model->create()->to_representation();
        }
        catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'update' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function update($root, $args, $context, $info): array {
        try {
            # Load the existing Model object with the given data
            $model = new $this->model(data: $args);

            # Return the update object in an array representation
            return $model->update()->to_representation();
        }
        catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'delete' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function delete($root, $args, $context, $info): array {
        try {
            # Load the existing Model object with the given data
            $model = new $this->model(data: $args);

            # Return the deleted object in an array representation
            return $model->delete()->to_representation();
        }
        catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'replace_all' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function replace_all($root, $args, $context, $info): array
    {
        try {
            # Load the existing Model object with the given data
            $model = new $this->model();

            # Return the replaced object in an array representation
            return $model->replace_all(data: $args['objects'])->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'delete_many' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function delete_many($root, $args, $context, $info): array
    {
        try {
            # Load the existing Model object with the given data
            $model = new $this->model();

            # Delete the objects that match our query
            $deleted_objects = $model->delete_many(
                query_params: $args['query_params'],
                limit: $args['limit'],
                offset: $args['offset'],
            );

            # Return the deleted objects in an array representation
            return $deleted_objects->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'delete_all' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function delete_all($root, $args, $context, $info): array {
        try {
            # Load the existing Model object with the given data
            $model = new $this->model();

            # Delete all objects
            $deleted_objects = $model->delete_all();

            # Return the deleted objects in an array representation
            return $deleted_objects->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }
}