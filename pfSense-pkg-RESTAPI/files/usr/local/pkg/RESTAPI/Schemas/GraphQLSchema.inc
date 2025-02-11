<?php

namespace RESTAPI\Schemas;

require_once 'RESTAPI/autoloader.inc';

use GraphQL\Type\Definition\CustomScalarType;
use GraphQL\Type\Definition\EnumType;
use GraphQL\Type\Definition\InputObjectType;
use GraphQL\Type\Definition\ObjectType;
use GraphQL\Type\Definition\Type;
use GraphQL\Utils\SchemaPrinter;
use RESTAPI\Core\Endpoint;
use RESTAPI\Core\Field;
use RESTAPI\Core\Model;
use RESTAPI\Core\Schema;
use RESTAPI\Fields\NestedModelField;
use RESTAPI\GraphQL\QueryParamsScalarType;
use RESTAPI\GraphQL\Resolver;
use RESTAPI\Responses\ServerError;
use function RESTAPI\Core\Tools\get_classes_from_namespace;
use function RESTAPI\Core\Tools\to_upper_camel_case;

/**
 * Defines a Schema that represents this API in GraphQL format. This class is used to generate a GraphQL schema file.
 */
class GraphQLSchema extends Schema {
    /**
     * @var string $file_name
     * The file name for this schema.
     */
    public string $file_name = 'graphql.gql';

    /**
     * @var array $field_enum_types
     * An array containing all the GraphQL EnumType objects that represent Fields in this API. The array key
     * is the EnumType name and the value is the GraphQL EnumType object.
     */
    public array $field_enum_types = [];

    /**
     * @var array $model_object_types
     * An array containing all the GraphQL ObjectType objects that represent Models in this API. The array key
     * is the Model class's FQN and the value is the GraphQL ObjectType object.
     */
    public array $model_object_types = [];

    /**
     * @var array $model_input_object_types
     * An array containing all the GraphQL InputObjectType objects that represent Models in this API. The array key
     * is the Model class's FQN and the value is the GraphQL InputObjectType object.
     */
    public array $model_input_object_types = [];

    /**
     * @var CustomScalarType $query_params_type A custom scalar type for arbitrary query parameters.
     */
    public CustomScalarType $query_params_type;

    /**
     * @inheritDoc
     */
    public function __construct() {
        # Create a custom scalar type for JSON arguments
        $this->query_params_type = new QueryParamsScalarType();

        parent::__construct();
    }

    /**
     * Obtains the top-level GraphQL Schema object.
     * @returns \GraphQL\Type\Schema The GraphQL Schema object for this API.
     */
    public function get_schema(): \GraphQL\Type\Schema {
        return new \GraphQL\Type\Schema([
            'query' => new ObjectType([
                'name' => 'Query',
                'fields' => $this->get_endpoint_query_types(),
            ]),
            'mutation' => new ObjectType([
                'name' => 'Mutation',
                'fields' => $this->get_endpoint_mutation_types(),
            ]),
            'types' => [$this->query_params_type],
        ]);
    }

    /**
     * @inheritDoc
     */
    public function get_schema_str(): string {
        return SchemaPrinter::doPrint($this->get_schema());
    }

    /**
     * Converts a Field object into a GraphQL schema field Type object.
     * @link https://webonyx.github.io/graphql-php/type-definitions/
     * @param Field $field The Field object to convert to a GraphQL schema field Type object.
     * @param Type|null $type A nested GraphQL schema field Type object that represents this Field's Type.
     * @param bool $ignore_required Whether this Field should be non-nullable or not.
     * @return Type The GraphQL schema field Type object that represents this Field.
     * @throws ServerError When this Field object has an unsupported type for GraphQL.
     */
    public function field_to_type(Field $field, ?Type $type = null, bool $ignore_required = false): Type {
        # Obtain the GraphQL type that corresponds to this Field's type if one was not provided
        $type =
            $type ?:
            match ($field->type) {
                'boolean' => Type::boolean(),
                'integer' => Type::int(),
                'double' => Type::float(),
                'string' => Type::string(),
                'array' => $this->query_params_type,
                default => throw new ServerError(
                    message: "Field '$field->name' has an unsupported type '$field->type' for GraphQL.",
                    response_id: 'FIELD_UNSUPPORTED_TYPE_FOR_GRAPHQL',
                ),
            };

        # Use an EnumType for Fields with choices
        if ($field->choices) {
            $type = $this->field_to_enum($field);
        }

        # Designate this field as a list if `many` is enabled
        if ($field->many) {
            $type = Type::listOf($type);
        }

        # Make this field non-nullable if it is required
        if ($field->required and !$ignore_required) {
            $type = Type::nonNull($type);
        }

        # Add a description to this field
        $type->description = $field->help_text;

        return $type;
    }

    /**
     * Converts a Field object into a GraphQL EnumType object. This turns a Field objects 'choices' into an EnumType.
     * @link https://webonyx.github.io/graphql-php/type-definitions/enums/
     * @param Field $field The Field object to convert to a GraphQL EnumType.
     * @return EnumType The GraphQL EnumType object that represents this Field.
     * @throws ServerError When this Field object does not have choices defined.
     */
    public function field_to_enum(Field $field): EnumType {
        # Variables
        $enum_name = $field->context->get_class_shortname() . to_upper_camel_case($field->name) . 'Enum';

        # If the Field has an EnumType already, return it
        if (array_key_exists($enum_name, $this->field_enum_types)) {
            # This value must be passed by reference to avoid duplicate/non-unique EnumTypes in the schema
            $enum_type = &$this->field_enum_types[$enum_name];
            return $enum_type;
        }

        # Define the enum values for this Field
        $values = [];
        foreach ($field->choices as $choice) {
            # Convert the choice to a string for EnumType
            $enum_key = strtoupper(strval($choice));

            # For empty values, convert to string for EnumType
            if ($choice === '') {
                $enum_key = 'EMPTY';
            }

            # Remove all special characters from the Enum key
            $enum_key = preg_replace('/[^A-Za-z0-9]/', '', $enum_key);

            # Prefix 'VAL_' to the Enum key to avoid any potential conflicts
            $enum_key = 'VAL_' . $enum_key;
            $values[$enum_key] = ['value' => $choice];
        }

        # Create a new EnumType object for this Field
        $this->field_enum_types[$enum_name] = new EnumType([
            'name' => $enum_name,
            'description' => $field->help_text,
            'values' => $values,
        ]);

        # This value must be passed by reference to avoid duplicate/non-unique EnumTypes in the schema
        $enum_type = &$this->field_enum_types[$enum_name];
        return $enum_type;
    }

    /**
     * Converts a given Model object to a GraphQL ObjectType or InputObjectType.
     * @param Model $model The Model object to convert to a GraphQL ObjectType or InputObjectType.
     * @param bool $input Whether this ObjectType should be an InputObjectType or not.
     * @return ObjectType|InputObjectType A GraphQL ObjectType definition for the given Model object.
     *@link https://webonyx.github.io/graphql-php/type-definitions/object-types/
     */
    public function model_to_object_type(Model $model, bool $input = false): ObjectType|InputObjectType {
        # Variables
        $fields = [];
        $model_sn = $model->get_class_shortname();
        $model_vn = $model->verbose_name;

        # If an ID is required, add it to the GraphQL schema
        if ($model->many) {
            $fields['id'] = $model->id_type === 'integer' ? Type::int() : Type::string();
            $fields['id'] = $input ? $fields['id'] : Type::nonNull($fields['id']);
        }

        # If a parent ID is required, add it to the GraphQL schema
        if ($model->parent_model_class) {
            $fields['parent_id'] =
                $model->parent_id_type === 'integer' ? Type::nonNull(Type::int()) : Type::nonNull(Type::string());
        }

        # Loop through each Field in this Model and obtain its GraphQL field definition
        foreach ($model->get_fields() as $field) {
            # For NestedModelFields, ensure we pass the Type for the nested Model
            if ($model->$field instanceof NestedModelField) {
                $nested_model_type = $this->get_model_input_object_type($model->$field->model_class);
                $fields[$field]['type'] = $this->field_to_type(field: $model->$field, type: $nested_model_type);
                $fields[$field]['description'] = $model->$field->help_text;
                continue;
            }

            # Generate and add the GraphQL field from the Field object
            $fields[$field]['type'] = $this->field_to_type(field: $model->$field);
            $fields[$field]['description'] = $model->$field->help_text;
        }

        return $input
            ? new InputObjectType(['name' => $model_sn . 'Input', 'description' => $model_vn, 'fields' => $fields])
            : new ObjectType(['name' => $model_sn, 'description' => $model_vn, 'fields' => $fields]);
    }

    /**
     * Obtains GraphQL ObjectType objects for all Models.
     * @returns array An array containing all the GraphQL ObjectType objects for Models in this API where the array key
     * is the Model's FQN and the value is the GraphQL ObjectType object. Note: This value can also be accessed via the
     * $model_object_types property.
     */
    public function get_model_object_types(): array {
        # Variables
        $model_classes = get_classes_from_namespace('RESTAPI\Models');

        # Reset any existing Types
        $this->model_object_types = [];

        # Loop through each Model and create a GraphQL Type object for each one.
        foreach ($model_classes as $model_class) {
            $this->model_object_types[$model_class] = $this->get_model_object_type($model_class);
        }

        return $this->model_object_types;
    }

    /**
     * Obtains GraphQL InputObjectType objects for all Models.
     * @returns array An array containing all the GraphQL InputObjectType objects for Models in this API where the array key
     * is the Model's FQN and the value is the GraphQL InputObjectType object. Note: This value can also be accessed via the
     * $model_input_object_types property.
     */
    public function get_model_input_object_types(): array {
        # Variables
        $model_classes = get_classes_from_namespace('RESTAPI\Models');

        # Reset any existing Types
        $this->model_input_object_types = [];

        # Loop through each Model and create a GraphQL InputObjectType object for each one.
        foreach ($model_classes as $model_class) {
            $this->model_input_object_types[$model_class] = $this->get_model_input_object_type($model_class);
        }

        return $this->model_input_object_types;
    }

    /**
     * Obtains the GraphQL ObjectType object for a given Model.
     * @param string $model_name The FQN of the Model to obtain the GraphQL Type object for.
     * @returns ObjectType The GraphQL Type object for the given Model.
     */
    public function get_model_object_type(string $model_name): ObjectType {
        # Append the Model's namespace if it doesn't exist.
        if (!str_starts_with($model_name, '\\RESTAPI\\Models\\')) {
            $model_name = '\\RESTAPI\\Models\\' . $model_name;
        }

        # Just return the existing Type if it already exists.
        if (array_key_exists($model_name, $this->model_object_types)) {
            # This value must be passed by reference to avoid duplicate/non-unique Types in the schema
            $type = &$this->model_object_types[$model_name];
            return $type;
        }

        # Otherwise, create the new Type, assign it to the array, and return it.
        $model = new $model_name(skip_init: true);
        $this->model_object_types[$model_name] = $this->model_to_object_type($model);

        # This value must be passed by reference to avoid duplicate/non-unique Types in the schema
        $type = &$this->model_object_types[$model_name];
        return $type;
    }

    /**
     * Obtains the GraphQL InputObjectType for a given Model.
     * @param string $model_name The FQN of the Model to obtain the GraphQL Type object for.
     * @returns Type The GraphQL InputObjectType object for the given Model.
     */
    public function get_model_input_object_type(string $model_name): InputObjectType {
        # Append the Model's namespace if it doesn't exist.
        if (!str_starts_with($model_name, '\\RESTAPI\\Models\\')) {
            $model_name = '\\RESTAPI\\Models\\' . $model_name;
        }

        # Just return the existing Type if it already exists.
        if (array_key_exists($model_name, $this->model_input_object_types)) {
            # This value must be passed by reference to avoid duplicate/non-unique Types in the schema
            $type = &$this->model_input_object_types[$model_name];
            return $type;
        }

        # Otherwise, create the new Type, assign it to the array, and return it.
        $model = new $model_name(skip_init: true);
        $this->model_input_object_types[$model_name] = $this->model_to_object_type($model, input: true);

        # This value must be passed by reference to avoid duplicate/non-unique Types in the schema
        $type = &$this->model_input_object_types[$model_name];
        return $type;
    }

    /**
     * Generates all GraphQL Query Types all Endpoint's with GET methods.
     * @returns array An array containing all the GraphQL Query Types for all Endpoints with GET methods in this API.
     */
    public function get_endpoint_query_types(): array {
        # Variables
        $query_types = [];

        # Loop through each Endpoint and create a Query Type for each one.
        foreach (get_classes_from_namespace('\\RESTAPI\\Endpoints\\') as $endpoint) {
            # Skip the GraphQLEndpoint
            if ($endpoint === '\RESTAPI\Endpoints\GraphQLEndpoint') {
                continue;
            }

            $endpoint = new $endpoint();
            $query_types = array_merge($query_types, $this->endpoint_to_queries($endpoint));
        }

        return $query_types;
    }

    /**
     * Generates all GraphQL Mutation Types all Endpoint's.
     * @returns array An array containing all the GraphQL Mutation Types for all Endpoints.
     */
    public function get_endpoint_mutation_types(): array {
        # Variables
        $mutation_types = [];

        # Loop through each Endpoint and create a Query Type for each one.
        foreach (get_classes_from_namespace('\\RESTAPI\\Endpoints\\') as $endpoint) {
            # Skip the GraphQLEndpoint
            if ($endpoint === '\RESTAPI\Endpoints\GraphQLEndpoint') {
                continue;
            }

            $endpoint = new $endpoint();
            $mutation_types = array_merge($mutation_types, $this->endpoint_to_mutations($endpoint));
        }

        return $mutation_types;
    }

    /**
     * Uses the URL from a given endpoint and converts it to a GraphQL query/mutation name with a given operation prefix.
     * @param string $operation The operation type/prefix for the GraphQL query/mutation.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL query/mutation name.
     * @returns array The GraphQL query field config for this Endpoint object.
     */
    public function endpoint_to_operation_name(string $operation, Endpoint $endpoint): string {
        # Ensure the operation name is lowercase
        $operation = lcfirst($operation);

        # Use the URL but remove the /api/v2/ prefix, and replace all slashes with spaces
        $name = str_replace('/api/v2/', '', $endpoint->url);
        $name = str_replace('/', ' ', $name);

        # Convert to camel case and add the operation prefix
        return $operation . to_upper_camel_case($name);
    }

    /**
     * Converts an Endpoint object into a GraphQL Query config. This method is responsible for defining the
     * GraphQL query operations that are applicable/equivalent to the Endpoint
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL query operation.
     * @returns array The GraphQL query field config for this Endpoint object.
     */
    public function endpoint_to_queries(Endpoint $endpoint): array {
        # Do not generate GraphQL query operations for Endpoints that do not allow GET requests
        if (!in_array('GET', $endpoint->request_method_options)) {
            return [];
        }

        # Get the query operation for this Endpoint. 'many' endpoints will get a query op, non-many will get a read op
        return $endpoint->many ? $this->get_query_query($endpoint) : $this->get_query_read($endpoint);
    }

    /**
     * Obtains a GraphQL 'query' operation config for a given Endpoint object. This is the GraphQL equivalent to a REST
     * API GET Endpoint that is 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL query operation.
     * @return array The GraphQL 'query' operation config for this Endpoint object.
     */
    public function get_query_query(Endpoint $endpoint): array {
        $resolver = new Resolver($endpoint->model);
        return [
            $this->endpoint_to_operation_name(operation: 'query', endpoint: $endpoint) => [
                'type' => Type::listOf($this->get_model_object_type($endpoint->model_name)),
                'resolve' => [$resolver, 'query'], // Use the GraphQLResolver::query method to resolve this query
                'args' => [
                    'query_params' => [
                        'type' => $this->query_params_type,
                        'defaultValue' => [],
                        'description' => 'An object containing the query parameters used to filter the results.',
                    ],
                    'limit' => [
                        'type' => Type::int(),
                        'defaultValue' => 0,
                        'description' => 'The maximum number of objects to return.',
                    ],
                    'offset' => [
                        'type' => Type::int(),
                        'defaultValue' => 0,
                        'description' => 'The offset to start returning objects from.',
                    ],
                    'reverse' => [
                        'type' => Type::boolean(),
                        'defaultValue' => false,
                        'description' => 'Reverse the order of the returned objects.',
                    ],
                    'sort_by' => [
                        'type' => Type::listOf(Type::string()),
                        'defaultValue' => [],
                        'description' => 'The fields to sort the returned objects by.',
                    ],
                    'sort_order' => [
                        'type' => Type::int(),
                        'defaultValue' => SORT_ASC,
                        'description' => 'The order to use when sorting.',
                    ],
                ],
            ],
        ];
    }

    /**
     * Obtains a GraphQL 'read' operation config for a given Endpoint object. This is the GraphQL equivalent to a REST
     * API GET Endpoint that is not 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL query operation.
     * @return array The GraphQL 'read' operation config for this Endpoint object.
     */
    public function get_query_read(Endpoint $endpoint): array {
        # Obtain our available resolvers and keep track arguments
        $resolver = new Resolver($endpoint->model);
        $args = [];

        # Require an ID if the Model is a many Model
        if ($endpoint->model->many) {
            $args['id'] = [
                'type' => $this->get_model_object_type($endpoint->model_name)->getField('id')->getType(),
                'description' => 'The ID of the object to read.',
            ];
        }
        # Require a parent ID if the Model has a parent Model
        if ($endpoint->model->parent_model_class) {
            $args['parent_id'] = [
                'type' => $this->get_model_object_type($endpoint->model_name)->getField('parent_id')->getType(),
                'description' => 'The parent ID of the object to read.',
            ];
        }

        return [
            $this->endpoint_to_operation_name(operation: 'read', endpoint: $endpoint) => [
                'type' => $this->get_model_object_type($endpoint->model_name),
                'resolve' => [$resolver, 'read'], // Use the GraphQLResolver::read method to resolve this query
                'args' => $args,
            ],
        ];
    }

    /**
     * Converts an Endpoint object into a GraphQL mutations config. This method is responsible for defining the
     * GraphQL mutation operations that are equivalent to the HTTP request methods supported by the Endpoint.
     * @param Endpoint $endpoint The Endpoint object to convert to GraphQL mutation operations.
     * @returns array The GraphQL mutations field config for the Endpoint object.
     */
    public function endpoint_to_mutations(Endpoint $endpoint): array {
        # Variables
        $mutations = [];

        # Include a mutation to create objects for non-many Endpoint's with POST request support
        if (!$endpoint->many and in_array('POST', $endpoint->request_method_options)) {
            $mutations = array_merge($mutations, $this->get_mutation_create($endpoint));
        }
        # Include a mutation to update objects for non-many Endpoint's with PATCH request support
        if (!$endpoint->many and in_array('PATCH', $endpoint->request_method_options)) {
            $mutations = array_merge($mutations, $this->get_mutation_update($endpoint));
        }
        # Include a mutation to delete objects for non-many Endpoint's with DELETE request support
        if (!$endpoint->many and in_array('DELETE', $endpoint->request_method_options)) {
            $mutations = array_merge($mutations, $this->get_mutation_delete($endpoint));
        }
        # Include a mutation to replace all objects for many Endpoint's with PUT request support
        if ($endpoint->many and in_array('PUT', $endpoint->request_method_options)) {
            $mutations = array_merge($mutations, $this->get_mutation_replace_all($endpoint));
        }
        # Include a mutation to delete many and/or all objects for many Endpoint's with DELETE request support
        if ($endpoint->many and in_array('DELETE', $endpoint->request_method_options)) {
            $mutations = array_merge($mutations, $this->get_mutation_delete_many($endpoint));
            $mutations = array_merge($mutations, $this->get_mutation_delete_all($endpoint));
        }

        return $mutations;
    }

    /**
     * Converts a Model object into a GraphQL mutation operation arguments config.
     * @param Model $model The Model object to convert to a GraphQL mutation operation arguments config.
     * @param bool $require_id Whether an ID is required for this mutation operation.
     * @param bool $only_id Whether only the ID is required for this mutation operation.
     * @param bool $ignore_required Whether to ignore required fields for this mutation operation.
     * @param array $exclude_fields An array of field names to exclude from the mutation operation.
     * @return array The GraphQL mutation operation arguments config for this Model object.
     */
    public function model_to_mutation_args(
        Model $model,
        bool $require_id = true,
        bool $only_id = false,
        bool $ignore_required = false,
        array $exclude_fields = [],
    ): array {
        # Variables
        $args = [];

        # If an ID is required, and the Model supports it, add it to the GraphQL schema
        if ($require_id) {
            $args['id'] = $model->id_type === 'integer' ? Type::nonNull(Type::int()) : Type::nonNull(Type::string());
        }

        # If the Model has a parent Model, add the parent ID to the GraphQL schema
        if ($model->parent_model_class) {
            $args['parent_id'] =
                $model->parent_id_type === 'integer' ? Type::nonNull(Type::int()) : Type::nonNull(Type::string());
        }

        # If only the ID is required, return the arguments now
        if ($only_id) {
            return $args;
        }

        # Loop through each Field in this Model and obtain its GraphQL field definition
        foreach ($model->get_fields() as $field) {
            # For NestedModelFields, ensure we pass the Type for the nested Model
            if ($model->$field instanceof NestedModelField) {
                $nested_model_type = $this->get_model_input_object_type($model->$field->model_class);
                $args[$field]['type'] = $this->field_to_type(
                    field: $model->$field,
                    type: $nested_model_type,
                    ignore_required: $ignore_required,
                );
                $args[$field]['description'] = $model->$field->help_text;
                continue;
            }

            # Generate and add the GraphQL field from the Field object
            $args[$field]['type'] = $this->field_to_type(field: $model->$field, ignore_required: $ignore_required);
            $args[$field]['description'] = $model->$field->help_text;
        }

        # Include control parameters for this mutation operation
        $args = array_merge($args, self::get_control_param_args());

        # Exclude any fields that are not required for this mutation operation
        return array_filter($args, fn($key) => !in_array($key, $exclude_fields), ARRAY_FILTER_USE_KEY);
    }

    /**
     * Obtains common control parameters as GraphQL mutation operation arguments
     * @returns array The control parameters for a GraphQL mutation operation.
     */
    public static function get_control_param_args(): array {
        return [
            'placement' => [
                'type' => Type::int(),
                'description' => 'The index to place this object in the configuration.',
            ],
            'append' => [
                'type' => Type::boolean(),
                'defaultValue' => false,
                'description' =>
                    'Force new array values to be appended to the existing array instead of replacing ' .
                    'it. This only applies to update operations.',
            ],
            'delete' => [
                'type' => Type::boolean(),
                'defaultValue' => false,
                'description' =>
                    'Force given array values to be removed from the existing array. This only applies ' .
                    'to update operations.',
            ],
            'apply' => [
                'type' => Type::boolean(),
                'defaultValue' => true,
                'description' => 'Apply configuration changes immediately where applicable.',
            ],
            'async' => [
                'type' => Type::boolean(),
                'defaultValue' => true,
                'description' => 'Run this operation asynchronously when possible.',
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to create a new object for the assigned Model.
     * This is the GraphQL equivalent to a REST API POST Endpoint that is not 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'create' mutation config for this Endpoint object.
     */
    protected function get_mutation_create(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        return [
            $this->endpoint_to_operation_name(operation: 'create', endpoint: $endpoint) => [
                'type' => $this->get_model_object_type($endpoint->model_name),
                'resolve' => [$resolver, 'create'], // Use the GraphQLResolver::create method to resolve this query
                'args' => $this->model_to_mutation_args(
                    model: $endpoint->model,
                    require_id: false,
                    ignore_required: true,
                    exclude_fields: ['append', 'delete'],
                ),
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to update a new object for the assigned Model.
     * This is the GraphQL equivalent to a REST API PATCH Endpoint that is not 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'update' mutation config for this Endpoint object.
     */
    protected function get_mutation_update(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        # Determine if this Model requires an ID to update
        $requires_id = $endpoint->model->many;

        return [
            $this->endpoint_to_operation_name(operation: 'update', endpoint: $endpoint) => [
                'type' => $this->get_model_object_type($endpoint->model_name),
                'resolve' => [$resolver, 'update'], // Use the Resolver::update method to resolve this query
                'args' => $this->model_to_mutation_args(
                    model: $endpoint->model,
                    require_id: $requires_id,
                    ignore_required: true,
                ),
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to delete a new object for the assigned Model.
     * This is the GraphQL equivalent to a REST API DELETE Endpoint that is not 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'delete' mutation config for this Endpoint object.
     */
    protected function get_mutation_delete(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        # Determine if this Model requires an ID to delete
        $requires_id = $endpoint->model->many;

        return [
            $this->endpoint_to_operation_name(operation: 'delete', endpoint: $endpoint) => [
                'type' => $this->get_model_object_type($endpoint->model_name),
                'resolve' => [$resolver, 'delete'], // Use the Resolver::delete method to resolve this query,
                'args' => $this->model_to_mutation_args(
                    model: $endpoint->model,
                    require_id: $requires_id,
                    ignore_required: true,
                    exclude_fields: ['append', 'delete', 'placement'],
                ),
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to replace all existing objects for the assigned Model.
     * This is the GraphQL equivalent to a REST API PUT Endpoint that is 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'replaceAll' mutation config for this Endpoint object.
     */
    protected function get_mutation_replace_all(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        return [
            $this->endpoint_to_operation_name(operation: 'replaceAll', endpoint: $endpoint) => [
                'type' => Type::listOf($this->get_model_object_type($endpoint->model_name)),
                'resolve' => [$resolver, 'replace_all'], // Use the Resolver::replace_all method to resolve this query,
                'args' => [
                    'objects' => [
                        'type' => Type::listOf($this->get_model_input_object_type($endpoint->model_name)),
                        'description' => 'The objects to replace all existing objects with.',
                    ],
                ],
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to delete many existing objects for the assigned
     * Model using a query. This is the GraphQL equivalent to a REST API DELETE Endpoint that is 'many' enabled.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'deleteMany' mutation config for this Endpoint object.
     */
    protected function get_mutation_delete_many(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        return [
            $this->endpoint_to_operation_name(operation: 'deleteMany', endpoint: $endpoint) => [
                'type' => Type::listOf($this->get_model_object_type($endpoint->model_name)),
                'resolve' => [$resolver, 'delete_many'], // Use the Resolver::delete_many method to resolve this query,
                'args' => [
                    'query_params' => [
                        'type' => Type::nonNull($this->query_params_type),
                        'description' =>
                            'An object containing the query parameters used to filter which objects to delete.',
                    ],
                    'limit' => [
                        'type' => Type::int(),
                        'defaultValue' => 0,
                        'description' => 'The maximum number of objects to delete.',
                    ],
                    'offset' => [
                        'type' => Type::int(),
                        'defaultValue' => 0,
                        'description' => 'The offset to start deleting objects from.',
                    ],
                ],
            ],
        ];
    }

    /**
     * Obtains a GraphQL mutation operation config that can be used to delete all existing objects for the assigned
     * Model. This is the GraphQL equivalent to a REST API DELETE Endpoint that is 'many' enabled and has the 'all`
     * parameter set to true.
     * @param Endpoint $endpoint The Endpoint object to convert to a GraphQL mutation operation.
     * @returns array The GraphQL 'deleteAll' mutation config for this Endpoint object.
     */
    protected function get_mutation_delete_all(Endpoint $endpoint): array {
        # Obtain our available resolvers
        $resolver = new Resolver($endpoint->model);

        return [
            $this->endpoint_to_operation_name(operation: 'deleteAll', endpoint: $endpoint) => [
                'type' => Type::listOf($this->get_model_object_type($endpoint->model_name)),
                'resolve' => [$resolver, 'delete_all'], // Use the Resolver::delete_all method to resolve this query,
                'args' => [],
            ],
        ];
    }
}
