<?php
//   Copyright 2023 Jared Hendrickson
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

use RESTAPI\Core\Model;
use RESTAPI\Core\Response;
use RESTAPI\Models\RESTAPISettings;

require_once("RESTAPI/autoloader.inc");

/**
 * Checks if a given array is an associative array.
 * @param array $array The array to check.
 * @return bool `true` if the value is an associative array. `false` if it is an indexed array.
 */
function is_assoc_array(array $array) : bool {
    # Start index tracking at 0
    $index = 0;

    # Loop through the keys of this array to check for non-associative indexing
    foreach (array_keys($array) as $key) {
        # This is an associtive array if the key is not an integer, or the index is not sequential
        if (!is_integer($key) or $key !== $index) {
            return true;
        }

        # Increase the index tracker
        $index++;
    }

    return false;
}

function http_request(
    string $url,
    string $method,
    array $data=[],
    array $headers=[],
    string $username="",
    string $password="",
    bool $validate_certs=true
) {
    # Format data and headers
    $data = json_encode($data);

    $headers["Content-Type"] = "application/json";
    $headers["Content-Length"] = strlen($data);

    # Setup cURL options
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, (int)$validate_certs);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, (int)$validate_certs);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, strtoupper($method));
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);

    # Only include basic authentication if username and password are set
    if ($username and $password) {
        curl_setopt($ch, CURLOPT_USERPWD, $username . ":" . $password);
    }

    # Make our request and return the response
    $result = curl_exec($ch);
    curl_close($ch);
    return $result;
}

function sync() {
    # Local variables
    $api_settings = new RESTAPISettings();
    $protocol = config_get_path("system/webgui/protocol", "https");
    $port = config_get_path("system/webgui/port", "443");

    # Only perform sync when HA Sync is enabled for the API
    if ($api_settings->ha_sync->value) {
        # Loop through each HA sync node and try to sync the configuration
        foreach ($api_settings->ha_sync_hosts->value as $node) {
            # Make the API request to sync configuration
            echo "Syncing API configuration to node ".$node."... ";
            $resp = http_request(
                url: $protocol."://".$node.":".$port."/api/v2/system/restapi/sync",
                method: "PUT",
                data: $api_settings->to_representation(),
                headers: [],
                username: $api_settings->ha_sync_username->value,
                password: $api_settings->ha_sync_password->value,
                validate_certs: $api_settings->ha_sync_validate_certs->value
            );

            # Ensure the sync was successful
            if ($resp) {
                $resp = json_decode($resp, true);
                # Check if our response was decoded correctly
                if ($resp) {
                    if ($resp["return"] === 0) {
                        echo "done." . PHP_EOL;
                    } elseif ($resp["return"] === 3) {
                        log_error("API sync failure (".$node."): Authentication failed");
                        echo "failed. Authentication failure.".PHP_EOL;
                    } elseif ($resp["return"] === 4) {
                        log_error("API sync failure (".$node."): Authorization failed");
                        echo "failed. Authorization failure.".PHP_EOL;
                    } else {
                        log_error("API sync failure (".$node."): Sync received return code ".strval($resp["return"]));
                        echo "failed. Received return code ".strval($resp["return"]).".".PHP_EOL;
                    }
                } else {
                    log_error("API sync failure (".$node."): Sync received unexpected response");
                    echo "failed. Received unexpected response.".PHP_EOL;
                }
            } else {
                log_error("API sync failure (".$node."): No response received from node");
                echo "failed. No response received from node.".PHP_EOL;
            }
        }
    } else {
        echo "Syncing API configuration... not configured.".PHP_EOL;
    }
}

/**
 * Obtains the classes nested under a specified namespace. This only works for namespaces that match the directory
 * structure (e.g. RESTAPI\Core for /etc/inc/RESTAPI/Core) and only includes classes defined in their own class .inc file with
 * a matching name.
 * @param string $namespace The namespace to obtain classes from. This must be the full namespace including leading
 * and trailing slash (\Full\Namespace\).
 * @return array An array containing the class names for each .inc class file.
 */
function get_classes_from_namespace(string $namespace) : array {
    # Variables
    $classes = [];
    $namespace_path = strtolower(
        str_replace(["\\", "//"], "/", "/etc/inc/".$namespace)
    );

    foreach(glob($namespace_path."*.inc") as $inc_file) {
        # Obtain the class name from the file name
        $class_name = basename($inc_file);
        $classes[] = str_replace(".inc", "", $namespace.$class_name);
    }

    return $classes;
}

/**
 * Auto-generates OpenAPI documentation for all Endpoints and their associated Models.
 */
function generate_documentation() {
    # Variables
    $endpoint_classes = get_classes_from_namespace("\\RESTAPI\\Endpoints\\");
    $response_classes = get_classes_from_namespace("\\RESTAPI\\Responses\\");

    # Set static documentation details
    $openapi_config = [
        "openapi" => "3.0.0",
        "servers" => [
            ["url" => "/", "description" => "This firewall"]
        ],
        "info" => [
            "title" => "pfSense REST API Documentation",
            "version" => "v2",
            "contact" =>[
                "name" => "GitHub",
                "url" => "https://github.com/jaredhendrickson13/pfsense-api",
            ],
            "license" => [
                "name" => "Apache 2.0",
                "url" => "https://raw.githubusercontent.com/jaredhendrickson13/pfsense-api/master/LICENSE"
            ],
            "description" => ""    # TODO: Pull this from the repos README.md
        ],
        "components" => [
            "Responses" => [],
            "schemas" => ["Response" => Response::to_openapi_schema()],
            "securitySchemes" => [
                "local" => ["type" => "http", "scheme" => "basic"],
                "jwt" => ["type" => "http", "scheme" => "bearer", "bearerFormat" => "JWT"],
                "key" => ["type" => "apiKey", "in" => "header", "name" => "Authorization"]
            ]
        ],
        "security" => [
            ["local" => []],
            ["jwt" => []],
            ["key" => []],
        ],
        "paths" => [],
        "tags" => []
    ];

    # Add Response components for each Response class in RESTAPI\Responses
    foreach($response_classes as $response_class) {
        # Create the Response object
        $response = new $response_class(message: "", response_id: "");
        $openapi_config["components"]["Responses"][$response->get_shortname()] = $response->to_openapi_component();
    }

    # Import each Endpoint class and assign documentation for the endpoint according to the options and Model assigned.
    foreach($endpoint_classes as $endpoint_class) {
        # Create the Response object
        $endpoint = new $endpoint_class();

        # Add this Endpoint's URL to the OpenAPI `paths`
        $openapi_config["paths"][$endpoint->url] = [];

        # Initialize this endpoint's OpenAPI tag
        $openapi_config["tags"][] = ["name" => $endpoint->tag];

        # Obtain the Model assigned to the Endpoint and create any assigned parent Model's
        $model = "RESTAPI\\Models\\$endpoint->model_name";
        $model = new $model();
        $model->get_parent_model();

        # Obtain the OpenAPI schema for this Model.
        $openapi_config["components"]["schemas"][$endpoint->model_name] = $model->to_openapi_schema();

        # Assign shared values to each request method defined in this path
        foreach ($endpoint->request_method_options as $request_method) {
            # Convert the request method to lower case so it matches the OpenAPI config
            $request_method = strtolower($request_method);

            # Obtain the privileges and help text associated with this request method
            $privilege_property = $request_method."_privileges";
            $help_text_property = $request_method."_help_text";

            # Assign endpoint details to variables
            $help_text = $endpoint->$help_text_property;
            $parent_model_class = ($model->parent_model_class) ?: "None";
            $priv_options_str = implode(", ", $endpoint->$privilege_property);
            $required_packages_str = ($model->packages) ? implode(", ", $model->packages) : "None";
            $requires_auth_str = ($endpoint->requires_auth) ? "Yes" : "No";
            $auth_modes_str = ($endpoint->enforce_auth_mode) ? "$endpoint->enforce_auth_mode" : "local, jwt, key";

            # Add documentation for all requests at this path
            $openapi_config["paths"][$endpoint->url][$request_method] = [
                "Responses" => [],
                "operationId" => $endpoint::class."-$request_method",
                "description" => "<h3>Description:</h3>".
                    "$help_text<br>".
                    "<h3>Details:</h3>".
                    "**Assigned model**: $endpoint->model_name<br>".
                    "**Parent model**: $parent_model_class<br>".
                    "**Requires authentication**: $requires_auth_str<br>".
                    "**Supported authentication modes:** [ $auth_modes_str ]<br>".
                    "**Allowed privileges**: [ $priv_options_str ]<br>".
                    "**Requires packages**: [ $required_packages_str ]"
            ];

            # Nest this endpoint under its assigned or assumed tag
            $openapi_config["paths"][$endpoint->url][$request_method]["tags"] = [$endpoint->tag];

            # Ensure the security mode is enforced for this path if the Endpoint has `enforce_auth_mode` set
            if ($endpoint->enforce_auth_mode) {
                $openapi_config["paths"][$endpoint->url][$request_method]["security"] = [
                    [$endpoint->enforce_auth_mode => []]
                ];
            }

            # Assign request body definitions for POST, PUT and PATCH requests
            if (in_array($request_method, ["post", "put", "patch"])) {
                # For non `many` Endpoints with `many` Models, add the ID to the schema and make it required
                if (!$endpoint->many and $model->many and $request_method !== "post") {
                    $schema = [
                        "schema" => [
                            "allOf" => [
                                [
                                    "type" => "object",
                                    "required" => ["id"],
                                    "properties" => [
                                        "id" => [
                                            "type" => "integer",
                                            "description" => "The ID of the object or resource to interact with."
                                        ]
                                    ]
                                ],
                                ['$ref' => "#/components/schemas/$endpoint->model_name"]
                            ]
                        ]
                    ];
                }
                # For `many` Endpoints with `many` Models, accept arrays of many schema objects
                elseif ($endpoint->many and $model->many) {
                    $schema = [
                        "schema" => [
                            "type" => "array", "items" => [
                                "allOf" => [
                                    ['$ref' => "#/components/schemas/$endpoint->model_name"],
                                    ["type" => "object", "required" => $model->get_fields(required_only: true)]
                                ]
                            ]
                        ]
                    ];
                }
                # Otherwise, just assign the schema with all required Fields included
                else {
                    $schema = [
                        "schema" => [
                            "allOf" => [
                                ['$ref' => "#/components/schemas/$endpoint->model_name"],
                                ["type" => "object", "required" => $model->get_fields(required_only: true)]
                            ]
                        ]
                    ];
                }
                
                # Add the `parent_id` field to Models with a `many` parent
                if ($model->parent_model_class and $model->parent_model->many) {
                    array_unshift($schema["schema"]["allOf"],  [
                        "type" => "object",
                        "required" => ["parent_id"],
                        "properties" => [
                            "parent_id" => [
                                "type" => "integer",
                                "description" => "The ID of the parent this object is nested under."
                            ]
                        ]
                    ]);
                }

                $openapi_config["paths"][$endpoint->url][$request_method]["requestBody"] = [
                    "content" => [
                        "application/json" => $schema,
                        "application/x-www-form-urlencoded" => $schema,
                    ]
                ];
            }

            # Assign the ID query parameter for GET and DELETE requests to non `many` Endpoints with a `many` Model assigned
            if (!$endpoint->many and $model->many and in_array($request_method, ["get", "delete"])) {
                $openapi_config["paths"][$endpoint->url][$request_method]["parameters"] = [
                    [
                        "in" => "query",
                        "name" => "id",
                        "description" => "The ID of the object to target.",
                        "required" => true,
                        "schema" => [
                            "oneOf" => [
                                ["type" => "integer"],
                                ["type" => "string"]
                            ]
                        ]
                    ]
                ];
                
                # Add the `parent_id` parameter if this model has a parent model assigned
                if ($model->parent_model_class and $model->parent_model->many) {
                    array_unshift(
                        $openapi_config["paths"][$endpoint->url][$request_method]["parameters"],
                        [
                            "in" => "query",
                            "name" => "parent_id",
                            "description" => "The ID of the parent this object is nested under.",
                            "required" => true,
                            "schema" => [
                                "oneOf" => [
                                    ["type" => "integer"],
                                    ["type" => "string"]
                                ]
                            ]
                        ]
                    );
                }

                # Add the `apply` parameter if this is a DELETE request
                if ($request_method == "delete" and $model->subsystem) {
                    $openapi_config["paths"][$endpoint->url][$request_method]["parameters"][] = [
                        "in" => "query",
                        "name" => "apply",
                        "description" => "Apply this deletion immediately.",
                        "required" => false,
                        "schema" => [
                            "type" => "bool",
                            "default" => false
                        ]
                    ];
                }
            }

            # Assign this request method Responses for each Response class defined.
            foreach ($response_classes as $response_class) {
                $response = new $response_class(message: "", response_id: "");
                $openapi_config["paths"][$endpoint->url][$request_method]["Responses"][$response->code] = [
                    '$ref' => "#/components/Responses/".$response->get_shortname()
                ];
            }
        }
    }

    return file_put_contents(
        filename: "/usr/local/www/RESTAPI/documentation/openapi.json",
        data: json_encode($openapi_config).PHP_EOL
    );
}
