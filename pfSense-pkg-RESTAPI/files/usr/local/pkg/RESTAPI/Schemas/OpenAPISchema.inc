<?php

namespace RESTAPI\Schemas;

use RESTAPI\Core\Schema;
use RESTAPI\Models\RESTAPIVersion;
use stdClass;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

class OpenAPISchema extends Schema {
    /**
     * @var string $file_name The name of the OpenAPI schema file.
     */
    public string $file_name = 'openapi.json';

    /**
     * @var string $content_type The content type for this schema. This is used to determine how the schema is formatted and displayed when
     * accessed via the URL.
     */
    public string $content_type = 'application/json';

    /**
     * Obtains the OpenAPI schema as a string.
     * @return string The OpenAPI schema as a string.
     */
    public function get_schema_str(): string {
        return json_encode($this->get_schema()) . PHP_EOL;
    }

    /**
     * Obtains the OpenAPI schema as an array.
     */
    public function get_schema(): array {
        # Variables
        $endpoint_classes = get_classes_from_namespace('\\RESTAPI\\Endpoints\\');
        $response_classes = get_classes_from_namespace('\\RESTAPI\\Responses\\');
        $auth_classes = get_classes_from_namespace('\\RESTAPI\\Auth\\');
        $auth_classes_short = get_classes_from_namespace('\\RESTAPI\\Auth\\', shortnames: true);
        $content_handler_classes = get_classes_from_namespace('\\RESTAPI\\ContentHandlers\\');
        $restapi_version = new RESTAPIVersion();
        $assigned_tags = [];

        # Set static openapi details
        $openapi_config = [
            'openapi' => '3.0.0',
            'servers' => [['url' => '/', 'description' => 'This firewall']],
            'info' => [
                'title' => 'pfSense REST API Documentation',
                'version' => $restapi_version->current_version->value,
                'contact' => [
                    'name' => 'GitHub',
                    'url' => 'https://github.com/jaredhendrickson13/pfsense-api',
                ],
                'license' => [
                    'name' => 'Apache 2.0',
                    'url' => 'https://raw.githubusercontent.com/jaredhendrickson13/pfsense-api/master/LICENSE',
                ],
                'description' =>
                    '### Getting Started' .
                    '<br>' .
                    '- [Authentication and Authorization](https://pfrest.org/AUTHENTICATION_AND_AUTHORIZATION/)<br>' .
                    '- [Working with Object IDs](https://pfrest.org/WORKING_WITH_OBJECT_IDS/)<br>' .
                    '- [Queries, Filters, and Sorting](https://pfrest.org/QUERIES_FILTERS_AND_SORTING/)<br>' .
                    '- [Common Control Parameters](https://pfrest.org/COMMON_CONTROL_PARAMETERS/)<br>' .
                    '- [Working with HATEOAS](https://pfrest.org/WORKING_WITH_HATEOAS/)<br>',
            ],
            'components' => [
                'schemas' => [],
                'securitySchemes' => [],
            ],
            'security' => [],
            'paths' => [],
            'tags' => [],
        ];

        # Add security and securitySchemes definitions for each Auth class
        foreach ($auth_classes as $auth_class) {
            # Create an object for this Auth class so we can obtain class information
            $auth_method = new $auth_class();
            $auth_shortname = $auth_method->get_class_shortname();

            # Add global security definitions.
            $openapi_config['security'][] = [$auth_shortname => []];

            # Add securitySchemes for each \RESTAPI\Auth class
            $openapi_config['components']['securitySchemes'][$auth_shortname] = $auth_method->security_scheme;
        }

        # Add Response components for each Response class in RESTAPI\Responses
        foreach ($response_classes as $response_class) {
            # Create the Response object
            $response = new $response_class(message: '', response_id: '');
            $resp_shortname = $response->get_class_shortname();

            # Add both a schema and response for this Response class
            $openapi_config['components']['schemas'][$resp_shortname] = $response->to_openapi_schema();
        }

        # Import each Endpoint class and assign openapi for the endpoint according to the options and Model assigned.
        foreach ($endpoint_classes as $endpoint_class) {
            # Create the Response object
            $endpoint = new $endpoint_class();

            # Add this Endpoint's URL to the OpenAPI `paths`
            $openapi_config['paths'][$endpoint->url] = [];

            # Initialize this endpoint's OpenAPI tag
            if (!in_array($endpoint->tag, $assigned_tags)) {
                $openapi_config['tags'][] = ['name' => $endpoint->tag];
                $assigned_tags[] = $endpoint->tag;
            }

            # Obtain the Model assigned to the Endpoint and create any assigned parent Model's
            $model = "\\RESTAPI\\Models\\$endpoint->model_name";
            $model = new $model();
            $model->get_parent_model();

            # Obtain the OpenAPI schema for this Model.
            $openapi_config['components']['schemas'][$endpoint->model_name] = $model->to_openapi_schema();

            # Assign shared values to each request method defined in this path
            foreach ($endpoint->request_method_options as $request_method) {
                # Convert the request method to lower case so it matches the OpenAPI config
                $request_method = strtolower($request_method);

                # Obtain the privileges and help text associated with this request method
                $privilege_property = $request_method . '_privileges';
                $help_text_property = $request_method . '_help_text';

                # Assign endpoint details to variables
                $help_text = $endpoint->$help_text_property;
                $endpoint_type = $endpoint->many ? 'Plural' : 'Singular';
                $parent_model_class = $model->parent_model_class ?: 'None';
                $priv_options_str = implode(', ', $endpoint->$privilege_property);
                $required_packages_str = $model->packages ? implode(', ', $model->packages) : 'None';
                $requires_auth_str = $endpoint->requires_auth ? 'Yes' : 'No';
                $auth_method_str = implode(', ', $endpoint->auth_methods ?: $auth_classes_short);
                $cache_class = $model->cache_class ?: 'None';
                $operation_id = "$request_method{$endpoint->get_class_shortname()}";

                # Determine if this Model applies changes immediately or not
                $applies_immediately = $model->does_apply_immediately();
                $applies_immediately = $applies_immediately ? 'Yes' : $applies_immediately;
                $applies_immediately = $applies_immediately === false ? 'No' : $applies_immediately;
                $applies_immediately = $applies_immediately === null ? 'Not Applicable' : $applies_immediately;

                # Add openapi for all requests at this path
                $openapi_config['paths'][$endpoint->url][$request_method] = [
                    'responses' => [],
                    'operationId' => $operation_id,
                    'deprecated' => $endpoint->deprecated,
                    'description' =>
                        '<h3>Description:</h3>' .
                        "$help_text<br>" .
                        '<h3>Details:</h3>' .
                        "**Endpoint type**: $endpoint_type<br>" .
                        "**Associated model**: $endpoint->model_name<br>" .
                        "**Parent model**: $parent_model_class<br>" .
                        "**Requires authentication**: $requires_auth_str<br>" .
                        "**Supported authentication modes:** [ $auth_method_str ]<br>" .
                        "**Allowed privileges**: [ $priv_options_str ]<br>" .
                        "**Required packages**: [ $required_packages_str ]<br>" .
                        "**Applies immediately**: $applies_immediately<br>" .
                        "**Utilizes cache**: $cache_class",
                ];

                # Nest this endpoint under its assigned or assumed tag
                $openapi_config['paths'][$endpoint->url][$request_method]['tags'] = [$endpoint->tag];

                # Ensure the security mode is enforced for this path if the Endpoint has `auth_methods` set
                if ($endpoint->auth_methods) {
                    foreach ($endpoint->auth_methods as $auth_method) {
                        $openapi_config['paths'][$endpoint->url][$request_method]['security'][] = [$auth_method => []];
                    }
                }

                # Assign request body definitions for POST, PUT and PATCH requests
                if (in_array($request_method, ['post', 'put', 'patch'])) {
                    # Only include required fields in the $allOf schema if there are required fields for this Model
                    $allof_schema = ['type' => 'object'];
                    $required_fields = $model->get_fields(required_only: true);
                    if ($required_fields) {
                        $allof_schema['required'] = $required_fields;
                    }

                    # For non `many` Endpoints with `many` Models, add the ID to the schema and make it required
                    if (!$endpoint->many and $model->many and $request_method !== 'post') {
                        $schema = [
                            'schema' => [
                                'allOf' => [
                                    [
                                        'type' => 'object',
                                        'required' => ['id'],
                                        'properties' => [
                                            'id' => [
                                                'type' => 'integer',
                                                'description' => 'The ID of the object or resource to interact with.',
                                            ],
                                        ],
                                    ],
                                    ['$ref' => "#/components/schemas/$endpoint->model_name"],
                                ],
                            ],
                        ];
                    }
                    # For `many` Endpoints with `many` Models, accept arrays of many schema objects
                    elseif ($endpoint->many and $model->many) {
                        # Write the schema objects with any required fields
                        $schema = [
                            'schema' => [
                                'type' => 'array',
                                'items' => [
                                    'allOf' => [
                                        ['$ref' => "#/components/schemas/$endpoint->model_name"],
                                        $allof_schema,
                                    ],
                                ],
                            ],
                        ];
                    }
                    # Otherwise, just assign the schema with all required Fields included
                    else {
                        $schema = [
                            'schema' => [
                                'allOf' => [['$ref' => "#/components/schemas/$endpoint->model_name"], $allof_schema],
                            ],
                        ];
                    }

                    # Add the `parent_id` field to Models with a `many` parent
                    if ($model->parent_model_class and $model->parent_model->many) {
                        array_unshift($schema['schema']['allOf'], [
                            'type' => 'object',
                            'required' => ['parent_id'],
                            'properties' => [
                                'parent_id' => [
                                    'type' => 'integer',
                                    'description' => 'The ID of the parent this object is nested under.',
                                ],
                            ],
                        ]);
                    }

                    # Populate OpenAPI 'content' definitions for each ContentHandler capable of decoding request bodies.
                    $contents = [];
                    foreach ($content_handler_classes as $content_handler_class) {
                        # Create an object for this content handler so we can extract handler info
                        $content_handler = new $content_handler_class();
                        $content_handler_sn = $content_handler->get_class_shortname();

                        # Skip this content handler if it's not allowed by the endpoint
                        if (
                            $endpoint->decode_content_handlers and
                            !in_array($content_handler_sn, $endpoint->decode_content_handlers)
                        ) {
                            continue;
                        }
                        # Skip this content handler if it can't decode
                        if (!$content_handler->can_decode()) {
                            continue;
                        }

                        $contents[$content_handler->mime_type] = $schema;
                    }
                    $openapi_config['paths'][$endpoint->url][$request_method]['requestBody']['content'] = $contents;
                }

                # Assign the ID query parameter for GET and DELETE requests to non `many` Endpoints with a `many` Model assigned
                if (!$endpoint->many and $model->many and in_array($request_method, ['get', 'delete'])) {
                    $openapi_config['paths'][$endpoint->url][$request_method]['parameters'] = [
                        [
                            'in' => 'query',
                            'name' => 'id',
                            'description' => 'The ID of the object to target.',
                            'required' => true,
                            'schema' => [
                                'oneOf' => [['type' => 'integer'], ['type' => 'string']],
                            ],
                        ],
                    ];

                    # Add the `parent_id` parameter if this model has a parent model assigned
                    if ($model->parent_model_class and $model->parent_model->many) {
                        array_unshift($openapi_config['paths'][$endpoint->url][$request_method]['parameters'], [
                            'in' => 'query',
                            'name' => 'parent_id',
                            'description' => 'The ID of the parent this object is nested under.',
                            'required' => true,
                            'schema' => [
                                'oneOf' => [['type' => 'integer'], ['type' => 'string']],
                            ],
                        ]);
                    }

                    # Add the `apply` parameter if this is a DELETE request
                    if ($request_method == 'delete' and $model->subsystem) {
                        $openapi_config['paths'][$endpoint->url][$request_method]['parameters'][] = [
                            'in' => 'query',
                            'name' => 'apply',
                            'description' => 'Apply this deletion immediately.',
                            'required' => false,
                            'schema' => [
                                'type' => 'boolean',
                                'default' => false,
                            ],
                        ];
                    }
                }

                # Assign the limit, offset, query and sort params to GET endpoints with $many enabled
                if ($endpoint->many and $request_method === 'get') {
                    $openapi_config['paths'][$endpoint->url][$request_method]['parameters'] = [
                        [
                            'in' => 'query',
                            'name' => 'limit',
                            'description' => 'The number of objects to obtain at once. Set to 0 for no limit.',
                            'schema' => ['type' => 'integer', 'default' => $endpoint->limit],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'offset',
                            'description' => 'The starting point in the dataset to begin fetching objects.',
                            'schema' => ['type' => 'integer', 'default' => $endpoint->offset],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'sort_by',
                            'description' => 'The fields to sort response data by.',
                            'schema' => [
                                'type' => 'array',
                                'items' => ['type' => 'string'],
                                'nullable' => true,
                                'default' => $endpoint->sort_by,
                            ],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'sort_order',
                            'description' => 'Sort the response data in descending order.',
                            'schema' => [
                                'type' => 'string',
                                'enum' => ['SORT_ASC', 'SORT_DESC'],
                                'nullable' => true,
                                'default' => $endpoint->sort_desc,
                            ],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'query',
                            'style' => 'form',
                            'explode' => true,
                            'description' =>
                                'The arbitrary query parameters to include in the request.<br><br>' .
                                'Note: This does not define an real parameter, rather it allows for any arbitrary query ' .
                                'parameters to be included in the request.',
                            'schema' => [
                                'type' => 'object',
                                'default' => new stdClass(),
                                'additionalProperties' => ['type' => 'string'],
                            ],
                        ],
                    ];
                }

                # Assign the limit and offset to DELETE endpoints with $many enabled
                if ($endpoint->many and $request_method === 'delete') {
                    $openapi_config['paths'][$endpoint->url][$request_method]['parameters'] = [
                        [
                            'in' => 'query',
                            'name' => 'limit',
                            'description' => 'The maximum number of objects to delete at once. Set to 0 for no limit.',
                            'schema' => ['type' => 'integer', 'default' => $endpoint->limit],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'offset',
                            'description' => 'The starting point in the dataset to begin fetching objects.',
                            'schema' => ['type' => 'integer', 'default' => $endpoint->offset],
                        ],
                        [
                            'in' => 'query',
                            'name' => 'query',
                            'style' => 'form',
                            'explode' => true,
                            'description' =>
                                'The arbitrary query parameters to include in the request.<br><br>' .
                                'Note: This does not define an actual parameter, rather it allows for any arbitrary query ' .
                                'parameters to be included in the request.',
                            'schema' => [
                                'type' => 'object',
                                'default' => new stdClass(),
                                'additionalProperties' => ['type' => 'string'],
                            ],
                        ],
                    ];
                }

                # Assign this request method Responses for each Response class defined.
                foreach ($response_classes as $response_class) {
                    # Construct the Response class
                    $response = new $response_class(message: '', response_id: '');
                    $response_sn = $response->get_class_shortname();
                    $content_handler_classes = get_classes_from_namespace('\\RESTAPI\\ContentHandlers\\');

                    # Skip this response class if it is not a response type for this endpoint
                    if ($endpoint->response_types and !in_array($response_sn, $endpoint->response_types)) {
                        continue;
                    }

                    # Loop through all ContentHandlers and add relevant accept types to the response
                    foreach ($content_handler_classes as $content_handler_class) {
                        # Create the content handler object and add the schema for the content to our component
                        $content_handler = new $content_handler_class();
                        $content_handler_sn = $content_handler->get_class_shortname();

                        # Skip this content handler if the Endpoint does not support it
                        if (
                            $endpoint->encode_content_handlers and
                            !in_array($content_handler_sn, $endpoint->encode_content_handlers)
                        ) {
                            continue;
                        }
                        # Skip content handlers that cannot encode
                        if (!$content_handler->can_encode()) {
                            continue;
                        }

                        $openapi_config['paths'][$endpoint->url][$request_method]['responses'][$response->code][
                            'description'
                        ] = $response->help_text;
                        $openapi_config['paths'][$endpoint->url][$request_method]['responses'][$response->code][
                            'content'
                        ] = [];
                        $openapi_config['paths'][$endpoint->url][$request_method]['responses'][$response->code][
                            'content'
                        ] = array_merge(
                            $openapi_config['paths'][$endpoint->url][$request_method]['responses'][$response->code][
                                'content'
                            ],
                            $content_handler->to_openapi_schema($response, $endpoint),
                        );
                    }
                }
            }
        }

        return $openapi_config;
    }
}
