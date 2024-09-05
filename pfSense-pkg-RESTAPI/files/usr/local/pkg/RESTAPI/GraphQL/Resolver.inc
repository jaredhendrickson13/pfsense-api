<?php

namespace RESTAPI\GraphQL;

require_once 'RESTAPI/autoloader.inc';

use GraphQL\Error\Error;
use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Core\Response;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Schemas\GraphQLSchema;

/**
 * Defines a class that maps/resolves GraphQL queries and mutations to the appropriate Model methods.
 */
class Resolver {
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
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'query', auth: $context['auth']);

            # Execute the query
            $query = $this->model->query(
                query_params: $args['query_params'],
                limit: $args['limit'],
                offset: $args['offset'],
                reverse: $args['reverse'],
            );

            # Return the query results in an array representation
            return $query->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL query to the Model's 'read' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function read($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'read', auth: $context['auth']);

            # Read the requested object using the Model's 'read' method (which is called on construction)
            $model = new $this->model(id: $args['id'] ?? null, parent_id: $args['parent_id'] ?? null);

            # Return the read results in an array representation
            return $model->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'create' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function create($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'create', auth: $context['auth']);

            # Create a new Model object with the given data
            $model = new $this->model(data: $args);

            # Return the created object in an array representation
            return $model->create()->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'update' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function update($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'update', auth: $context['auth']);

            # Load the existing Model object with the given data
            $model = new $this->model(data: $args);

            # Return the update object in an array representation
            return $model->update()->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'delete' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function delete($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'delete', auth: $context['auth']);

            # Load the existing Model object with the given data
            $model = new $this->model(data: $args);

            # Return the deleted object in an array representation
            return $model->delete()->to_representation();
        } catch (Response $e) {
            throw new Error(message: $e->message, extensions: ['response_id' => $e->response_id]);
        }
    }

    /**
     * A resolver that maps a GraphQL mutation to the Model's 'replace_all' method.
     * @link https://webonyx.github.io/graphql-php/schema-definition-language/#defining-resolvers
     */
    public function replace_all($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'replace_all', auth: $context['auth']);

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
    public function delete_many($root, $args, $context, $info): array {
        try {
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'delete_many', auth: $context['auth']);

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
            # Ensure that the user has the required privileges to perform this action
            $this->check_privs(resolver: 'delete_all', auth: $context['auth']);

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

    /**
     * Checks if authenticated user has privileges to perform the requested action.
     * @param string $resolver The resolver method to check permissions for.
     * @param Auth $auth The Auth object to use for checking permissions.
     * @throws ForbiddenError If the user does not have the required privileges.
     */
    public function check_privs(string $resolver, Auth $auth): void {
        # Obtain the endpoint that corresponds with the Model and resolve method
        switch ($resolver) {
            case 'query':
                $endpoint = $this->model->get_related_endpoint(many: true);
                $auth->required_privileges = $endpoint->get_privileges;
                break;
            case 'read':
                $endpoint = $this->model->get_related_endpoint(many: false);
                $auth->required_privileges = $endpoint->get_privileges;
                break;
            case 'create':
                $endpoint = $this->model->get_related_endpoint(many: false);
                $auth->required_privileges = $endpoint->post_privileges;
                break;
            case 'update':
                $endpoint = $this->model->get_related_endpoint(many: false);
                $auth->required_privileges = $endpoint->patch_privileges;
                break;
            case 'delete':
                $endpoint = $this->model->get_related_endpoint(many: false);
                $auth->required_privileges = $endpoint->delete_privileges;
                break;
            case 'replace_all':
                $endpoint = $this->model->get_related_endpoint(many: true);
                $auth->required_privileges = $endpoint->put_privileges;
                break;
            case 'delete_all':
            case 'delete_many':
                $endpoint = $this->model->get_related_endpoint(many: true);
                $auth->required_privileges = $endpoint->delete_privileges;
                break;
            default:
                throw new ForbiddenError(
                    message: 'Unknown GraphQL resolver action requested.',
                    response_id: 'GRAPHQL_RESOLVER_UNKNOWN_CHECK_PRIVS_ACTION',
                );
        }

        # Throw an error if the user does not have the required privileges
        if (!$auth->authorize()) {
            throw new ForbiddenError(
                message: 'Authorization failed. You do not have sufficient privileges to access this resource.',
                response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            );
        }
    }
}
