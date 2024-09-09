<?php

namespace RESTAPI\GraphQL;

require_once 'RESTAPI/autoloader.inc';
require_once 'RESTAPI/.resources/vendor/webonyx/graphql-php/src/Type/Definition/CustomScalarType.php';

use GraphQL\Error\Error;
use GraphQL\Language\AST\ObjectValueNode;
use GraphQL\Language\AST\ValueNode;
use GraphQL\Type\Definition\CustomScalarType;

/**
 * Defines a custom scalar type for handling JSON objects in GraphQL queries. This is primarily intended to allow
 * arbitrary objects to be passed as arguments to GraphQL queries and mutations. This scalar type should only be
 * used if the schema of the objects is dynamic and cannot be easily represented by a GraphQL input object type.
 */
class QueryParamsScalarType extends CustomScalarType {
    public function __construct() {
        parent::__construct([
            'name' => 'QueryParams',
            'description' =>
                'The `queryParam` scalar type represents arbitrary GraphQL object containing query parameters as key-value pairs.',
            'serialize' => function ($value) {
                # In order to print the schema, this cannot be an object so just use an empty string
                return '';
            },
            'parseValue' => function ($value) {
                # Only handle PHP arrays or objects directly
                if (is_array($value) or is_object($value)) {
                    return (array) $value;
                }
                throw new Error('Cannot parse value as a QueryParams object.');
            },
            'parseLiteral' => function ($ast) {
                # Only handle GraphQL objects
                if ($ast instanceof ObjectValueNode === false) {
                    throw new Error('Invalid QueryParams literal. Expected value to be an object.');
                }

                # Keep track of fields and values
                $result = [];

                # Iterate over each field in the object
                foreach ($ast->fields as $field) {
                    $value = $field->value;

                    # Handle nested objects
                    if ($value instanceof ObjectValueNode) {
                        # Recursively handle any nested objects
                        $result[$field->name->value] = $this->parseLiteral($value);
                    }
                    # Handle primitive values
                    elseif ($value instanceof ValueNode) {
                        $result[$field->name->value] = $value->value;
                    }
                }
                return $result;
            },
        ]);
    }
}
