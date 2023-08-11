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

namespace API\Core\Tools;

require_once("api/auto_loader.inc");

use Firebase\JWT\JWT;
use Firebase\JWT\Key;

function api_request($url, $method, $data=[], $headers=[], $username="", $password="") {
    # Format data and headers
    $data = json_encode($data);

    # Ensure headers is always an array
    if (!is_array($headers)) {
        $headers = [];
    }
    $headers["Content-Type"] = "application/json";
    $headers["Content-Length"] = strlen($data);

    # Setup cURL options
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
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
    $pkg_conf = get_api_config()[1];
    $protocol = config_get_path("system/webgui/protocol", "https");
    $port = config_get_path("system/webgui/port", "443");

    # Only perform sync when HA Sync is enabled for the API
    if (isset($pkg_conf["hasync"])) {
        # Loop through each HA sync node and try to sync the configuration
        foreach (explode(" ", $pkg_conf["hasync_hosts"]) as $node) {
            # Make the API request to sync configuration
            echo "Syncing API configuration to node ".$node."... ";
            $resp = api_request(
                $protocol."://".$node.":".$port."/api/v1/system/api/sync",
                "PUT",
                $pkg_conf,
                $pkg_conf["hasync_username"],
                $pkg_conf["hasync_password"]
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
 * Auto-generates OpenAPI documentation for all Views and their associated Models.
 */
function generate_documentation() {
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
            "responses" => [
                "Success" => [
                    "description" => "Client has made a successful API call",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "AuthenticationFailed" => [
                    "description" => "Client has failed to authenticate the API call.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "Forbidden" => [
                    "description" => "Client does not have sufficient privileges to make this API call.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "ValidationError" => [
                    "description" => "Client request has one or more syntax errors.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "NotFound" => [
                    "description" => "Client has requested a resource that does not exist.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "ServerError" => [
                    "description" => "Server encountered an unexpected error.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ],
                "UndefinedError" => [
                    "description" => "This response has not been pre-defined in the OpenAPI configuration.",
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => '#/components/schemas/Response']
                        ]
                    ]
                ]
            ],
            "schemas" => [
                "Response" => [
                    "type" => "object",
                    "properties" => [
                        "code" => [
                            "description" => "The HTTP status code that corresponds with the API response.",
                            "type" => "integer"
                        ],
                        "status" => [
                            "description" => "The HTTP status message that corresponds with the HTTP status code.",
                            "type" => "string"
                        ],
                        "response_id" => [
                            "description" => "The unique response ID that corresponds with the result of the API".
                                "call. In most situations, this will contain an error code.",
                            "type" => "string"
                        ],
                        "message" => [
                            "description" => "The descriptive message detailing the results of the API call.",
                            "type" => "string"
                        ],
                        "data" => [
                            "description" => "The data requested from the API. In the event that many objects have".
                                "been requested, this field will be an array of objects. Otherwise, it will only return".
                                "the single object requested.",
                            "example" => [],
                            "oneOf" => [
                                ["type" => "array"],
                                ["type" => "object"]
                            ]
                        ],
                    ],
                ]
            ],
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

    # Import each View class and assign documentation for the endpoint according to the options and Model assigned.
    foreach(glob("/etc/inc/api/views/*.inc") as $file) {
        # Import classes files and create the View object
        require_once($file);
        $view_class = "\\API\\Views\\" . str_replace(".inc", "", basename($file));
        $view_obj = new $view_class();

        # Add this View's URL to the OpenAPI `paths`
        $openapi_config["paths"][$view_obj->url] = [];

        # Set the tag for this URL according to it's root namespace (the section in URL after /api/v2/)
        $view_tag = strtoupper(explode("/", $view_obj->url)[3]);
        $openapi_config["tags"][] = ["name" => $view_tag];

        # Obtain the Model assigned to the View
        $model = "API\\Models\\$view_obj->model_name";
        $model = new $model();

        # Obtain the OpenAPI schema for this model
        $openapi_config["components"]["schemas"][$view_obj->model_name] = $model->to_openapi_schema();

        # Assign shared values to each request method defined in this path
        foreach ($view_obj->request_method_options as $request_method) {
            # Convert the request method to lower case so it matches the OpenAPI config
            $request_method = strtolower($request_method);
            $privilege_property = $request_method."_privileges";
            $help_text_property = $request_method."_help_text";

            # Assign endpoint details to variables
            $help_text = $view_obj->$help_text_property;
            $priv_options_str = implode(", ", $view_obj->$privilege_property);
            $required_packages_str = ($model->packages) ? implode(", ", $model->packages) : "None";
            $requires_auth_str = ($view_obj->requires_auth) ? "Yes" : "No";
            $auth_modes_str = ($view_obj->enforce_auth_mode) ? "$view_obj->enforce_auth_mode" : "local, jwt, key";

            # Add documentation for GET requests at this path
            $openapi_config["paths"][$view_obj->url][$request_method] = [
                "operationId" => $view_obj::class."-$request_method",
                "description" => "<h3>Description:</h3>".
                    "$help_text<br>".
                    "<h3>Details:</h3>".
                    "**Assigned model**: $view_obj->model_name<br>".
                    "**Requires authentication**: $requires_auth_str<br>".
                    "**Supported authentication modes:** [ $auth_modes_str ]<br>".
                    "**Allowed privileges**: [ $priv_options_str ]<br>".
                    "**Requires packages**: [ $required_packages_str ]"
            ];

            # Assign tags to this path
            $openapi_config["paths"][$view_obj->url][$request_method]["tags"] = [$view_tag];

            # Ensure the security mode is enforced for this path if the View has `enforce_auth_mode` set
            if ($view_obj->enforce_auth_mode) {
                $openapi_config["paths"][$view_obj->url][$request_method]["security"] = [
                    [$view_obj->enforce_auth_mode => []]
                ];
            }

            # Assign request body schemas to POST and PATCH requests
            if (in_array($request_method, ["post", "patch"])) {
                $openapi_config["paths"][$view_obj->url][$request_method]["requestBody"] = [
                    "content" => [
                        "application/json" => [
                            "schema" => ['$ref' => "#/components/schemas/$view_obj->model_name"]
                        ],
                        "application/x-www-form-urlencoded" => [
                            "schema" => ['$ref' => "#/components/schemas/$view_obj->model_name"]
                        ],
                    ]
                ];
            }

            # Assign responses to each request method
            $openapi_config["paths"][$view_obj->url][$request_method]["responses"] = [
                "200" => ['$ref' => '#/components/responses/Success'],
                "400" => ['$ref' => '#/components/responses/ValidationError'],
                "401" => ['$ref' => '#/components/responses/AuthenticationFailed'],
                "403" => ['$ref' => '#/components/responses/Forbidden'],
                "404" => ['$ref' => '#/components/responses/NotFound'],
                "500" => ['$ref' => '#/components/responses/ServerError'],
                "default" => ['$ref' => '#/components/responses/UndefinedError'],
            ];
        }
    }

    return json_encode($openapi_config).PHP_EOL;
}
