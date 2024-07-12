<?php
//   Copyright 2024 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

namespace RESTAPI\Core\Tools;

use RESTAPI\Core\Response;
use RESTAPI\Models\RESTAPIVersion;
use RESTAPI\Responses\ServerError;

require_once 'RESTAPI/autoloader.inc';

/**
 * Checks if a given array is an associative array.
 * @param array $array The array to check.
 * @return bool `true` if the value is an associative array. `false` if it is an indexed array.
 */
function is_assoc_array(array $array): bool {
    # Start index tracking at 0
    $index = 0;

    # Loop through the keys of this array to check for non-associative indexing
    foreach (array_keys($array) as $key) {
        # This is an associative array if the key is not an integer, or the index is not sequential
        if (!is_integer($key) or $key !== $index) {
            return true;
        }

        # Increase the index tracker
        $index++;
    }

    return false;
}

/**
 * Conditionally prints a message to the console. The specified $message will only be printed if $condition is true.
 * @param string $message The message to be printed if the $condition is true.
 * @param bool $condition A true or false condition. If the condition is true, the $message will be printed. Otherwise
 * it will not.
 */
function cprint(string $message, bool $condition): void {
    if ($condition) {
        print $message;
    }
}

/**
 * Converts a $bandwidth to bits for it's given $bandwidthtype
 * @param int $bandwidth The amount of bandwidth scaled using $bandwidthtype.
 * @param string $scale The scale of $bandwidth. Options are: b, Kb, Mb, and Gb.
 * @return int The bandwidth's total number of bits.
 * @throws ServerError When $bandwidthtype is not a supported option.
 */
function bandwidth_to_bits(int $bandwidth, string $scale): int {
    # Set scale factors we can use to convert various bandwidth values into total bit count
    $gigabit_scale_factor = 1024 * 1024 * 1024;
    $megabit_scale_factor = 1024 * 1024;
    $kilobit_scale_factor = 1024;

    return match ($scale) {
        'Gb' => $bandwidth * $gigabit_scale_factor,
        'Mb' => $bandwidth * $megabit_scale_factor,
        'Kb' => $bandwidth * $kilobit_scale_factor,
        'b' => $bandwidth, // Already represented as bits
        default => throw new ServerError(
            message: "Unsupported \$bandwidthtype value `$scale` provided to bandwidth_to_bits(). " .
                'Supported options are b, Kb, Mb, and Gb.',
            response_id: 'TOOLS_BANDWIDTH_TO_BITS_INVALID_SCALE_OPTION',
        ),
    };
}

/**
 * Makes an HTTP request.
 * @param string $url The full URL of the resource to submit the request to.
 * @param string $method The HTTP method to use to submit the request.
 * @param array $data The data payload to submit to the remote host. Only JSON payloads are supported.
 * @param array $headers The HTTP headers to submit in the request.
 * @param string $username The username portion for basic authentication (if applicable)
 * @param string $password The password portion for basic authentication (if applicable)
 * @param bool $validate_certs Enables or disables TLS certificate verification.
 * @return bool|string The response from the remote host.
 */
function http_request(
    string $url,
    string $method,
    array $data = [],
    array $headers = [],
    string $username = '',
    string $password = '',
    bool $validate_certs = true,
): bool|string {
    # Format data and headers
    $data = json_encode($data);

    $headers[] = 'content-type: application/json';
    $headers[] = 'content-length: ' . strlen($data);

    # Setup cURL options
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, (int) $validate_certs);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, (int) $validate_certs);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, strtoupper($method));
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);

    # Only include basic authentication if username and password are set
    if ($username and $password) {
        curl_setopt($ch, CURLOPT_USERPWD, $username . ':' . $password);
    }

    # Make our request and return the response
    $result = curl_exec($ch);
    curl_close($ch);
    return $result;
}

/**
 * Obtains the classes nested under a specified namespace. This only works for namespaces that match the directory
 * structure (e.g. RESTAPI\Core for /usr/local/pkg/RESTAPI/Core) and only includes classes defined in their own class .inc file with
 * a matching name.
 * @param string $namespace The namespace to obtain classes from. This must be the full namespace including leading
 * and trailing slash (\Full\Namespace\).
 * @param bool $shortnames Obtain the class shortnames instead of FQNs.
 * @return array An array containing the class names for each .inc class file.
 */
function get_classes_from_namespace(string $namespace, bool $shortnames = false): array {
    # Variables
    $classes = [];
    $namespace = !str_starts_with($namespace, '\\') ? $namespace . '\\' : $namespace;
    $namespace = !str_starts_with($namespace, '\\') ? '\\' . $namespace : $namespace;
    $namespace_path = str_replace(['\\', '//'], '/', '/usr/local/pkg/' . $namespace);

    foreach (glob($namespace_path . '*.inc') as $inc_file) {
        # Obtain the class name from the file name
        $class_name = basename($inc_file);
        $class_name = str_replace('.inc', '', $class_name);
        $classes[] = $shortnames ? $class_name : $namespace . $class_name;
    }

    return $classes;
}

/**
 * Auto-generates OpenAPI documentation for all Endpoints and their associated Models.
 */
function generate_documentation(): bool {
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
                '- [Queries and Filters](https://pfrest.org/QUERIES_AND_FILTERS/)<br>' .
                '- [Common Control Parameters](https://pfrest.org/COMMON_CONTROL_PARAMETERS/)<br>' .
                '- [Working with HATEOAS](https://pfrest.org/WORKING_WITH_HATEOAS/)<br>',
        ],
        'components' => [
            'responses' => [],
            'schemas' => ['Response' => Response::to_openapi_schema()],
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
        $openapi_config['components']['responses'][$resp_shortname] = $response->to_openapi_component();
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
            $applies_immediately = $model->always_apply ? 'Yes' : 'No';
            $cache_class = $model->cache_class ?: 'None';
            $operation_id = "$request_method{$endpoint->get_class_shortname()}";

            # Add openapi for all requests at this path
            $openapi_config['paths'][$endpoint->url][$request_method] = [
                'responses' => [],
                'operationId' => $operation_id,
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
                                'allOf' => [['$ref' => "#/components/schemas/$endpoint->model_name"], $allof_schema],
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

                    # Only include this ContentHandler as a valid content definition if it can decode data
                    if ($content_handler->can_decode()) {
                        $contents[$content_handler->mime_type] = $schema;
                    }
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

            # Assign the limit and offset to GET endpoints with $many enabled
            if ($endpoint->many and $request_method === 'get') {
                $openapi_config['paths'][$endpoint->url][$request_method]['parameters'] = [
                    [
                        'in' => 'query',
                        'name' => 'limit',
                        'description' => 'The number of objects to obtain at once. Set to 0 for no limit.',
                        'schema' => ['type' => 'integer', 'default' => 0],
                    ],
                    [
                        'in' => 'query',
                        'name' => 'offset',
                        'description' => 'The starting point in the dataset to begin fetching objects.',
                        'schema' => ['type' => 'integer', 'default' => 0],
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
                            'default' => '{}',
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
                        'schema' => ['type' => 'integer', 'default' => 0],
                    ],
                    [
                        'in' => 'query',
                        'name' => 'offset',
                        'description' => 'The starting point in the dataset to begin fetching objects.',
                        'schema' => ['type' => 'integer', 'default' => 0],
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
                            'default' => '{}',
                            'additionalProperties' => ['type' => 'string'],
                        ],
                    ],
                ];
            }

            # Assign this request method Responses for each Response class defined.
            foreach ($response_classes as $response_class) {
                $response = new $response_class(message: '', response_id: '');
                $openapi_config['paths'][$endpoint->url][$request_method]['responses'][$response->code] = [
                    '$ref' => '#/components/responses/' . $response->get_class_shortname(),
                ];
            }
        }
    }

    return (bool) file_put_contents(
        filename: '/usr/local/pkg/RESTAPI/.resources/schema.json',
        data: json_encode($openapi_config) . PHP_EOL,
    );
}
