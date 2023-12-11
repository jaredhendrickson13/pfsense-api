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
namespace RESTAPI\Core;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Responses\MediaTypeError;
use RESTAPI\Responses\MethodNotAllowedError;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\AuthenticationError;
use RESTAPI\Responses\Success;
use RESTAPI\Responses\ValidationError;
use Error;
use Exception;
use JsonException;

/**
 * Represents an API Endpoint, encapsulating attributes essential for generating the API endpoint
 * and facilitating API calls. description is not adequate.
 */
class Endpoint {
    /**
     * @var string $url
     * The URL of this endpoint, used to generate a PHP file in the web path pointing
     * back to this Endpoint class.
     */
    public string $url = "";

    /**
     * @var string $model_name
     * The name of the Model class this Endpoint interacts with (excluding the Model
     * class's namespace).
     */
    public string $model_name = "";

    /**
     * @var Model $model
     * The actual Model object created for the assigned $model_name.
     */
    protected Model $model;

    /**
     * @var string $tag
     * The OpenAPI tag applied to this Endpoint, nesting it under a specific name in
     * the OpenAPI documentation. Defaults to the first value in the $url after
     * /RESTAPI/v2/.
     */
    public string $tag = "";

    /**
     * @var array $errors
     * An array to store errors encountered during API calls to this Endpoint.
     */
    protected array $errors = [];

    /**
     * @var Auth $client
     * The \RESTAPI\Core\Auth object created by this Endpoint during API calls, contains
     * client authentication information.
     */
    protected Auth $client;

    /**
     * @var array $api_config
     * Contains the current API configuration.
     */
    public array $api_config;

    /**
     * @var array $request_data
     * The request body or parameters sent by the remote client.
     */
    public array $request_data = [];

    /**
     * @var bool $many
     * Indicates whether this Endpoint interacts with a single Model object (false) or
     * many Model objects (true).
     */
    public bool $many = false;

    /**
     * @var string $request_method
     * The HTTP request method sent by the remote client.
     */
    public string $request_method;

    /**
     * @var array $request_method_options
     * Sets the allowed HTTP request methods for this Endpoint. For $many=true, only
     * GET and PUT methods can be specified here. GET will use the Model's `read_all()`
     * method, and PUT will use the Model's `replace_all()` method. For Endpoints with
     * $many=false, GET, POST, PATCH, and DELETE methods can be specified here. GET will
     * use the Model's `read()` method, POST will use the Model's `create()` method.
     * PATCH will use the Model's `update()` method, and DELETE will use the Model's
     * `delete()` method.
     */
    public array $request_method_options = [];

    /**
     * @property bool $requires_auth
     * Specify whether this Endpoint should require remote clients to authenticate
     * before allowing them to interact with the assigned Model's objects. In almost
     * every situation, this should be set to `true`!
     */
    public bool $requires_auth = true;

    /**
     * @var string $enforce_auth_mode
     * Explicitly specify an API authentication mode clients MUST use when making
     * requests to this Endpoint. Leave blank to use the authentication mode specified
     * in the API settings.
     */
    public string $enforce_auth_mode = "";

    /**
     * @var bool $ignore_read_mode
     * Allow this Endpoint to ignore the current `read_only` API setting. By default,
     * when the `read_only` API setting is enabled, Endpoints are only allowed to
     * respond to GET requests. Setting this value to `true` will allow requests using
     * any HTTP method even if `read_only` is enabled.
     */
    public bool $ignore_read_mode = false;

    /**
     * @var bool $ignore_interfaces
     * Allow this Endpoint to ignore the `allowed_interfaces` API setting. By default,
     * if an Endpoint receives an API call on an interface not specified in the
     * `allowed_interfaces` API setting, a ForbiddenError will be thrown. Setting this
     * value to `true` will allow this Endpoint to respond to requests over any
     * interface regardless of what is specified in `allowed_interfaces`.
     */
    public bool $ignore_interfaces = false;

    /**
     * @var bool $ignore_enabled
     * Allow this Endpoint to ignore the `enabled` API setting. By default, Endpoints
     * will not accept API calls when the API's `enabled` setting is disabled. Setting
     * this value to `true` will allow this Endpoint to respond to API calls even if
     * the API is disabled.
     */
    public bool $ignore_enabled = false;

    /**
     * @var array $get_privileges
     * Assigns pfSense privileges that should allow GET requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $get_privileges = [];

    /**
     * @var array $post_privileges
     * Assigns pfSense privileges that should allow POST requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $post_privileges = [];

    /**
     * @var array $patch_privileges
     * Assigns pfSense privileges that should allow PATCH requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $patch_privileges = [];

    /**
     * @var array $put_privileges
     * Assigns pfSense privileges that should allow PUT requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $put_privileges = [];

    /**
     * @var array $delete_privileges
     * Assigns pfSense privileges that should allow DELETE requests to this Endpoint.
     * This property is dynamically populated.
     */
    public array $delete_privileges = [];

    /**
     * @var string $get_help_text
     * Sets the GET request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $get_help_text = "";

    /**
     * @var string $post_help_text
     * Sets the POST request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $post_help_text = "";

    /**
     * @var string $put_help_text
     * Sets the PUT request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $put_help_text = "";

    /**
     * @var string $patch_help_text
     * Sets the PATCH request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $patch_help_text = "";

    /**
     * @var string $delete_help_text
     * Sets the DELETE request's OpenAPI documentation for this Endpoint. This will be
     * used when generating the OpenAPI documentation for this Endpoint. This is
     * typically only necessary if the default description is not adequate.
     */
    public string $delete_help_text = "";

    /**
     * Sets values whenever the Endpoint object is crated.
     */
    public function __construct() {
        # Prevent PHP warnings in API Responses.
        error_reporting(E_ERROR);

        # Fetch the current API configuration whenever the object is first created.
        $this->api_config = RESTAPI\Models\RESTAPISettings::get_api_config()["config"];

        # Fetch request method whenever the object is first created.
        $this->request_method = strtoupper($_SERVER["REQUEST_METHOD"]);

        # Initialize the assigned model
        $model_class = "RESTAPI\\Models\\".$this->model_name;
        $this->model = new $model_class();

        # Set the default OpenAPI tag for this endpoint if no tag was explicitly assigned
        $this->set_default_tag();

        # Set default help text for any help text properties that were not set by the child class
        $this->set_default_help_text();

        # Check for conflicting attributes
        $this->check_construct();

        # Set default endpoint privileges
        $this->get_default_privs();
    }

    /**
     * Checks for conflicting attributes during object construction.
     */
    private function check_construct(): void
    {
        # Do not allow `many` Endpoints that are assigned non `many` Models
        if ($this->many and !$this->model->many) {
            throw new ServerError(
                message: "Endpoints cannot enable `many` when the assigned Model is not a `many` Model",
                response_id: "ENDPOINT_MANY_WITHOUT_MANY_MODEL"
            );
        }

        # Do not allow Endpoints to allow unsupported request methods
        $supported_methods = ($this->many) ? ["GET", "PUT"] : ["GET", "POST", "PATCH", "DELETE"];
        foreach ($this->request_method_options as $method_option) {
            if (!in_array($method_option, $supported_methods)) {
                throw new ServerError(
                    "Endpoint contains unsupported request method option `$method_option`.",
                    response_id: "ENDPOINT_WITH_UNSUPPORTED_REQUEST_METHOD_OPTION"
                );
            }
        }
        
        # Do not allow `many` Endpoints that are assigned a Model with a parent Model
        if ($this->many and $this->model->parent_model_class) {
            throw new ServerError(
                message: "Endpoints cannot enable `many` when the assigned Model has a parent Model",
                response_id: "ENDPOINT_MANY_WHEN_MODEL_HAS_PARENT"
            );
        }
    }

    /**
     * Use the endpoint's URL to determine a default OpenAPI tag for this endpoint.
     */
    private function set_default_tag(): void
    {
        # Only assume the default if a tag was not set
        if (!$this->tag) {
            # Set the tag for this URL according to it's root namespace (the section in URL after /RESTAPI/v2/)
            $this->tag = strtoupper(explode("/", $this->url)[3]);
        }
    }

    /**
     * Determines the appropriate default help text for each method according to the assigned Model and Endpoint properties.
     */
    private function set_default_help_text() {
        # Variables
        $model_verbose_name = $this->model->verbose_name;
        $model_verbose_name_plural = $this->model->verbose_name_plural;

        # For non `many` Endpoints, set default help text with an applicable context for `many` Models
        if (!$this->many and $this->model->many) {
            $this->get_help_text = ($this->get_help_text) ?: "Reads an existing $model_verbose_name.";
            $this->post_help_text = ($this->post_help_text) ?: "Creates a new $model_verbose_name.";
            $this->patch_help_text = ($this->patch_help_text) ?: "Updates an existing $model_verbose_name.";
            $this->delete_help_text = ($this->delete_help_text) ?: "Deletes an existing $model_verbose_name.";
        }
        # For `many` Vies, set default help text with an applicable context for `many` Models
        elseif ($this->many and $this->model->many) {
            $this->get_help_text = ($this->get_help_text) ?: "Reads all existing $model_verbose_name_plural.";
            $this->put_help_text = ($this->put_help_text) ?: "Replaces all existing $model_verbose_name_plural.";
        }
        # Set default help text with an applicable context for non `many` Models
        else {
            $this->get_help_text = ($this->get_help_text) ?: "Reads current $model_verbose_name_plural.";
            $this->post_help_text = ($this->post_help_text) ?: "Creates $model_verbose_name_plural.";
            $this->patch_help_text = ($this->patch_help_text) ?: "Updates current $model_verbose_name_plural.";
            $this->delete_help_text = ($this->delete_help_text) ?: "Deletes current $model_verbose_name_plural.";
        }
    }

    /**
     * Obtains the API key or JWT as a bearer token from the client.
     * @returns String A string containing the API token or the JWT from the client.
     */
    private function get_bearer_token() : string {
        # Obtain the API key or JWT from the authorization header.
        $auth_header = explode(" ", $_SERVER["HTTP_AUTHORIZATION"]);
        $token_type = strtolower($auth_header[0]);
        $secret = ($auth_header[1]) ?: "";

        # Ensure this API key or JWT was passed in as a bearer token
        if ($token_type === "bearer") {
            return $secret;
        }

        # Return an empty string if there were any problems obtaining the secret
        return "";
    }

    /**
     * Generates the privilege name necessary to execute a specific request method on this Endpoint.
     * @param string $method The HTTP method for which the resulting privilege name is intended.
     * @returns string The complete pfSense privilege name needed to authorize API requests to this endpoint using 
     * the specified $method.
     */
    private function get_method_priv_name(string $method) : string {
        # Convert the URL and request method to a pfSense privilege name
        $priv_name_prefix = str_replace("/", "-", $this->url)."-";
        
        # Replace underscores with hyphens
        $priv_name_prefix = str_replace("_", "-", $priv_name_prefix);
        
        # Remove leading `-` if present, this occurs because the URL starts with a `/` which gets replaced with `-`
        if (str_starts_with($priv_name_prefix, "-")) {
            $priv_name_prefix = substr($priv_name_prefix, offset: 1);
        }
        
        return $priv_name_prefix.strtolower($method);
    }

    /**
     * Assigns the default privileges required to make API calls to this endpoint. This method will automatically
     * populate the following properties:
     * 
     *      - $this->get_privileges
     *      - $this->post_privileges
     *      - $this->patch_privileges
     *      - $this->put_privileges
     *      - $this->delete_privileges
     */
    private function get_default_privs() : void {
        $page_all_priv = "page-all";
        $this->get_privileges = [$page_all_priv, $this->get_method_priv_name("GET")];
        $this->post_privileges = [$page_all_priv, $this->get_method_priv_name("POST")];
        $this->patch_privileges = [$page_all_priv, $this->get_method_priv_name("PATCH")];
        $this->put_privileges = [$page_all_priv, $this->get_method_priv_name("PUT")];
        $this->delete_privileges = [$page_all_priv, $this->get_method_priv_name("DELETE")];
    }

    /**
     * Retrieves the necessary privileges for processing the received API call.
     * @return array An array of required privileges for the current API call.
     */
    private function get_required_privs() : array {
        # Determine the privileges required for this API call
        if ($this->request_method === "GET") {
            return $this->get_privileges;
        }
        elseif ($this->request_method === "POST") {
            return $this->post_privileges;
        }
        elseif ($this->request_method === "PUT") {
            return $this->put_privileges;
        }
        elseif ($this->request_method === "PATCH") {
            return $this->patch_privileges;
        }
        elseif ($this->request_method === "DELETE") {
            return $this->delete_privileges;
        }
        else {
            return [];
        }
    }

    /**
     * Utilizes the $url and $request_method_options properties of this Endpoint to generate pfSense privileges
     * specific to this Endpoint class. The resulting array is then supplied to the pfSense privilege system
     * located at /etc/inc/priv/.
     * @returns array The pfSense priv list entry array corresponding to the privileges of this Endpoint.
     */
    public function generate_pfsense_privs() : array  {
        # Set an array to populate pfSense priv entries for this Endpoint
        $privs = [];

        # Loop through each support request method and create a pfSense privilege entry for it
        foreach ($this->request_method_options as $method_option) {
            # Set the privilege config to pass to pfSense's privilege system
            $privs[$this->get_method_priv_name($method_option)] = [
                "name" => "API - $this->url ".strtoupper($method_option),
                "descr" => "Allow $method_option request to $this->url"
            ];
        }

        return $privs;
    }

    /**
     * Checks if the API client is authenticated and authorized to make this call.
     */
    private function check_auth() {
        # Variables
        $read_only = ($this->api_config["read_only"] === "enabled" and !$this->ignore_read_mode);
        $this->client = new Auth($this->get_required_privs(), $this->enforce_auth_mode, $read_only);

        # Obtain the auth credentials from client according to the current API auth mode
        switch ($this->client->mode) {
            case "local":
                $this->client->username = ($_SERVER["PHP_AUTH_USER"]) ?: "";
                $secret = ($_SERVER["PHP_AUTH_PW"]) ?: "";
                break;
            case "jwt":
                $secret = $this->get_bearer_token();
                break;
            case "key":
                $secret = ($_SERVER["HTTP_AUTHORIZATION"]) ?: "";
                break;
            default:
                throw new ServerError(
                    message: "Unknown authentication mode configured.",
                    response_id: "ENDPOINT_WITH_UNKNOWN_AUTH_MODE"
                );
        }

        # Only proceed if this a view that requires auth
        if ($this->requires_auth === true) {
            if (!$this->client->authenticate(secret: $secret, username: $this->client->username)) {
                throw new AuthenticationError(
                    message: "Authentication failed",
                    response_id: "INVALID_AUTH"
                );
            }
            if (!$this->client->authorize()) {
                throw new ForbiddenError(
                    message: "Client does not have sufficient privileges to access this resource.",
                    response_id: "INSUFFICIENT_PRIVILEGES"
                );
            }
        }
    }

    /**
     * Checks if the API is enabled before allowing the call.
     */
    private function check_enabled() {
        if (!$this->api_config["enabled"] == "enabled" and !$this->ignore_enabled) {
            header("Location: /");
            exit();
        }
    }

    /**
     * Checks if the interface receiving the API call is allowed to answer API calls.
     */
    private function check_server_ip() {
        # Variables
        $allow_ifs = $this->api_config["allowed_interfaces"];
        $whitelist = explode(",", $allow_ifs);

        # Allow the action if the allowed interfaces is empty
        if (empty($allow_ifs)) {
            return;
        }

        # Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            # Check if our server IP is a valid if address, VIP, localhost, or any
            if ($_SERVER["SERVER_ADDR"] === $if_info["ipaddr"]) {
                return;
            } elseif ($_SERVER["SERVER_ADDR"] === $if_info["ipaddrv6"]) {
                return;
            } elseif (in_array($_SERVER["SERVER_ADDR"], ["::1", "127.0.0.1", "localhost"]) and $wif === "localhost") {
                return;
            } elseif ($wif === "any") {
                return;
            } elseif ($this->ignore_interfaces) {
                return;
            }
        }

        # Throw a forbidden error if this API call was made to a non-API enabled interface
        throw new ForbiddenError(
            message: "The requested action is not allowed by admin policy",
            response_id: "NOT_ALLOWED"
        );
    }

    /**
     * Check if the HTTP method requested by the API client is supported by this view.
     */
    private function check_request_method() {
        # Check if the requested HTTP method is a supported HTTP method.
        if (!in_array($this->request_method, $this->request_method_options) and $this->request_method !== "OPTIONS") {
            throw new MethodNotAllowedError(
                message: "Resource at $this->url does not support the requested HTTP method '$this->request_method'",
                response_id: "METHOD_NOT_ALLOWED"
            );
        }
    }

    /**
     * Checks if the API client's request data was successfully received and parsed by the API. This method populates
     * the $this->request_data property according to the client's requested content-type.
     */
    private function check_request_data() {
        # Variables
        $content_type_unknown = false;
        $content_type = $_SERVER["HTTP_CONTENT_TYPE"];

        # Assign the default content type for the request method if no content type was specified
        if (!$content_type) {
            $content_type = match ($this->request_method) {
                "GET", "DELETE" => "application/x-www-form-urlencoded",
                default => "application/json",
            };
        }

        # Parse the content according to the content-type request header
        switch ($content_type) {
            case "application/x-www-form-urlencoded":
                $this->request_data = $this->decode_form_content($_GET);
                break;
            case "application/json":
                $this->request_data = $this->decode_json_content(file_get_contents('php://input'));
                break;
            default:
                $content_type_unknown = true;
        }

        # Check if the client requested an unknown content type
        if ($content_type_unknown) {
            throw new MediaTypeError(
                message: "The requested content-type is not supported",
                response_id: "ENDPOINT_CONTENT_TYPE_UNSUPPORTED"
            );
        }
    }

    /**
     * Listens for incoming API calls from API clients and checks if the API call is allowed.
     */
    public function listen() {
        # Variables
        $resp_data = null;

        try {
            # Ensure API is enabled and the receiving interface is allowed to answer API calls.
            $this->check_server_ip();

            # Checks client authentication and authorization if required
            if ($this->requires_auth) {
                $this->check_auth();
            }

            # Checks that request data was properly received and parsed.
            $this->check_request_method();
            $this->check_request_data();

            # Assign the authenticated client object context to the Model
            $this->model->client = $this->client;

            # For GET requests on non-many Endpoints, obtain a single object from the assigned Model.
            if ($this->request_method === "GET" and !$this->many) {
                $this->model->from_representation(data: $this->request_data);
                $resp_data = $this->model->read();
            }
            # For GET requests on many Endpoints, obtain all objects from the assigned Model.
            elseif ($this->request_method === "GET" and $this->many) {
                $resp_data = $this->model->query(query_params: $this->request_data);
            }
            # For POST requests on non-many Endpoints, create a single object for the assigned Model
            elseif ($this->request_method === "POST" and !$this->many) {
                # POST request cannot include an ID, strip the ID if present
                unset($this->request_data["id"]);

                $this->model->from_representation(data: $this->request_data);
                $resp_data = $this->model->create(apply: ($this->request_data["apply"] === true));
            }
            # For PUT requests on many Endpoints, replace all existing objects for the assigned Model
            elseif ($this->request_method === "PUT" and $this->many) {
                $resp_data = $this->model->replace_all(
                    data: $this->request_data,
                    apply: ($this->request_data["apply"] === true)
                );
            }
            # For PATCH requests on non-many Endpoints, update a single object for the assigned Model
            elseif ($this->request_method === "PATCH" and !$this->many) {
                $this->model->from_representation(data: $this->request_data);
                $resp_data = $this->model->update(apply: ($this->request_data["apply"] === true));
            }
            # For DELETE requests on non-many Endpoints, delete a single object for the assigned Model
            elseif ($this->request_method === "DELETE" and !$this->many) {
                $this->model->from_representation(data: $this->request_data);
                $resp_data = $this->model->delete(apply: ($this->request_data["apply"] === true));
            }
            # Return the available request methods in a response header
            elseif ($this->request_method === "OPTIONS") {
                header("access-control-allow-methods: ".implode(", ", $this->request_method_options));
            }

            # Create our success API response object if no errors were thrown
            $response = new Success(
                message: "",
                response_id: "SUCCESS",
                data: ($resp_data) ? $resp_data->to_representation() : []
            );
        }
        catch (Response $response) {
        }
        catch (Exception|Error $error) {
            $response = new ServerError(
                message: $error->getMessage().PHP_EOL.$error->getTraceAsString(),
                response_id: "ENDPOINT_RECEIVED_UNEXPECTED_ERROR"
            );
        }

        # Add API required response headers, these will override any custom headers
        header("Referer: no-referrer");

        # Format the HTTP response as JSON and set response code
        http_response_code($response->code);
        echo $this->content_type_encode($response->to_representation()) . PHP_EOL;
        session_destroy();
        exit();
    }

    /**
     * Encodes the API response according to the Accept header received by the API client.
     * @param array $data The API response data to encode in the requested format.
     * @return string The encoded API response or `false` if the data could not be encoded.
     */
    public function content_type_encode(array $data) : string {
        # TODO: in future if additional response Content-Types need to be supported we can add them here
        # TODO: and control the requested content-type using the Accept HTTP header
        # Encode response as JSON if content type is application/json
        header("Content-Type: application/json", true);
        return json_encode($data);
    }

    /**
     * Attempts to decode a given JSON string into a PHP array. This is essentially a wrapper for the default
     * `json_decode()` PHP function that allows us to apply some additional logic.
     */
    public static function decode_json_content(string $json_string) : array {
        # Allow empty strings, but convert it to an empty array.
        if (empty($json_string)) {
            return [];
        }

        # Try to decode the JSON string, throw a not acceptable error if the string is not JSON encoded.
        try {
            return json_decode($json_string, associative: true, flags: JSON_THROW_ON_ERROR);
        }
        catch (JsonException) {
            throw new ValidationError(
                message: "Request body could not be decoded as JSON.",
                response_id: "ENDPOINT_COULD_NOT_DECODE_JSON"
            );
        }
    }

    /**
     * Applies primitive type assumption to application/x-www-form-urlencoded request data.
     * @param array $array The form data array to decode.
     * @return array The parsed API client's request data
     */
    public static function decode_form_content(array $array) : array {
        # Loop through each query string item and check for expected data types
        foreach ($array as $key=>$value) {
            # Check for boolean type query strings
            if ($value === "true") {
                $array[$key] = true;
            }
            elseif ($value === "false") {
                $array[$key] = false;
            }

            # Check for integer type query strings
            if (is_numeric($value)) {
                $array[$key] = intval($value);
            }
        }

        return $array;
    }

    /**
     * Generate an endpoint for this view in the pfSense webroot.
     */
    public function build_endpoint_url() {
        # Get the fully qualified and non qualified class names for this object
        $fq_class_name = get_class($this);
        $nq_class_name = (new \ReflectionClass($this))->getShortName();

        # Specify the PHP code to write to the Endpoints index.php file
        $code = "<?php\nrequire_once('RESTAPI/Endpoints/".$nq_class_name.".inc');\n(new ".$fq_class_name."())->listen();\n";

        # Create directories and files corresponding with class
        if ($this->url) {
            mkdir("/usr/local/www".$this->url, 0755, true);
            file_put_contents(
                "/usr/local/www".$this->url."/index.php",
                $code
            );
        }

        # Print success output if file now exists, otherwise output error and exit on non-zero code
        if ($this->url and is_file("/usr/local/www".$this->url."/index.php")) {
            echo "Building ".$fq_class_name." at URL \"".$this->url."\"... done.".PHP_EOL;
        } else {
            echo "Building ".$fq_class_name." at URL \"".$this->url."\"... failed.".PHP_EOL;
            exit(1);
        }
    }
}
