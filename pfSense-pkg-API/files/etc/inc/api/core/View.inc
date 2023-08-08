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
namespace API\Core;

require_once("api/core/Model.inc");

use API;
use Error;
use Exception;
use JsonException;

# Allow endpoints to access any API model class
foreach(glob("/etc/inc/api/models/*.inc") as $model) {
    require_once($model);
}

/**
 * Creates a View object that contains all metrics required to generate an API endpoint and facilitate API calls.
 */
class View {
    public string $url = "";
    protected string $model_name = "";
    protected mixed $model = null;
    protected array $errors = [];
    protected API\Core\Auth $client;
    public array $api_config = [];
    public array $request_data = [];
    public string $request_method;
    public array $request_method_options = [];
    public bool $requires_auth = true;
    public string $enforce_auth_mode = "";
    public bool $ignore_read_mode = false;
    public bool $ignore_interfaces = false;
    public bool $ignore_enabled = false;
    public array $get_privileges = [];
    public array $post_privileges = [];
    public array $patch_privileges = [];
    public array $delete_privileges = [];

    /**
     * Sets values whenever the View object is crated.
     */
    public function __construct() {
        # Prevent PHP warnings in API responses.
        error_reporting(E_ERROR);

        # Fetch the current API configuration whenever the object is first created.
        $this->api_config = API\Core\Tools\get_api_config()[1];

        # Fetch request method whenever the object is first created.
        $this->request_method = strtoupper($_SERVER["REQUEST_METHOD"]);
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
     * Checks the required privileges based on the request method.
     * @return array an array of privileges required for the API call
     */
    private function get_required_privs() : array {
        # Determine the privileges required for this API call
        if ($this->request_method === "GET") {
            return $this->get_privileges;
        }
        elseif ($this->request_method === "POST") {
            return $this->post_privileges;
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
     * Checks if the API client is authenticated and authorized to make this call.
     */
    private function check_auth() {
        # Variables
        $read_only = (isset($this->api_config["readonly"]) and !$this->ignore_read_mode);
        $this->client = new API\Core\Auth($this->get_required_privs(), $this->enforce_auth_mode, $read_only);

        # Obtain the auth credentials from client according to the current API auth mode
        switch ($this->api_config["authmode"]) {
            case "local":
                $this->client->username = ($_SERVER["PHP_AUTH_USER"]) ?: "";
                $secret = ($_SERVER["PHP_AUTH_PW"]) ?: "";
                break;
            case "jwt" or "key":
                $secret = $this->get_bearer_token();
                break;
            default:
                throw new ServerError(
                    message: "Unknown authentication mode configured.",
                    ref_id: "VIEW_WITH_UNKNOWN_AUTH_MODE"
                );
        }

        # Only proceed if this a view that requires auth
        if ($this->requires_auth === true) {
            if (!$this->client->authenticate(secret: $secret, username: $this->client->username)) {
                throw new AuthenticationError(
                    message: "Authentication failed",
                    ref_id: "INVALID_AUTH"
                );
            }
            if (!$this->client->authorize()) {
                throw new ForbiddenError(
                    message: "Client does not have sufficient privileges to access this resource.",
                    ref_id: "INSUFFICIENT_PRIVILEGES"
                );
            }
        }
    }

    /**
     * Checks if the API is enabled before allowing the call.
     */
    private function check_enable() {
        $api_config = API\Core\Tools\get_api_config()[1];
        if (!isset($api_config["enable"]) and !$this->ignore_enabled) {
            header("Location: /");
            exit();
        }
    }

    /**
     * Checks if the interface receiving the API call is allowed to answer API calls.
     */
    private function check_server_ip() {
        $allow_ifs = $this->api_config["allowed_interfaces"];
        $whitelist = explode(",", $allow_ifs);

        # Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            # Check if our server IP is a valid if address, VIP, localhost, or any
            if ($_SERVER["SERVER_ADDR"] === $if_info["ipaddr"] or API\Core\Tools\is_ip_vip($_SERVER["SERVER_ADDR"], $wif)) {
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
            ref_id: "NOT_ALLOWED"
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
                ref_id: "METHOD_NOT_ALLOWED"
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

        # Parse the content according to the content-type request header
        switch ($_SERVER["HTTP_CONTENT_TYPE"]) {
            case "application/json":
                $this->request_data = $this->decode_json_content(file_get_contents('php://input'));
                break;
            case "application/x-www-form-urlencoded":
                $this->request_data = $this->decode_form_content($_GET);
                break;
            default:
                $content_type_unknown = true;
        }

        # Check if the client requested an unknown content type
        if ($content_type_unknown) {
            throw new ValidationError(
                message: "The requested content-type is not supported",
                ref_id: "VIEW_CONTENT_TYPE_UNSUPPORTED",
                code: 415
            );
        }
    }

    /**
     * Listens for incoming API calls from API clients and checks if the API call is allowed.
     */
    public function listen() {
        try {
            # Ensure API is enabled and the receiving interface is allowed to answer API calls.
            $this->check_enable();
            $this->check_server_ip();

            # Checks client authentication and authorization if required
            if ($this->requires_auth) {
                $this->check_auth();
            }

            # Checks that request data was properly received and parsed.
            $this->check_request_method();
            $this->check_request_data();

            # Initialize the assigned model
            $this->model_name = "API\\Models\\".$this->model_name;
            $this->model = new $this->model_name(
                representation_data: $this->request_data,
                client: $this->client
            );

            # Define a variable to store our response data (if any)
            $resp_data = [];

            # Run the associated model method based on the incoming HTTP method
            if ($this->request_method === "GET") {
                $resp_data = $this->model->read(all: true);
            }
            elseif ($this->request_method === "POST") {
                $resp_data = $this->model->create(apply: ($this->request_data["apply"] === true));
            }
            elseif ($this->request_method === "PATCH") {
                $resp_data = $this->model->update(apply: ($this->request_data["apply"] === true));
            }
            elseif ($this->request_method === "DELETE") {
                $resp_data = $this->model->delete(apply: ($this->request_data["apply"] === true));
            }
            elseif ($this->request_method === "OPTIONS") {
                header("access-control-allow-methods: ".implode(", ", $this->request_method_options));
            }

            # Create our success API response object if no errors were thrown
            $response = new Response(
                message: "",
                ref_id: "SUCCESS",
                code: 200,
                data: $resp_data
            );
        }
        catch (Response $response) {
        }
        catch (Exception|Error $error) {
            $response = new ServerError(
                message: $error->getMessage(),
                ref_id: "VIEW_RECEIVED_UNEXPECTED_ERROR"
            );
        }

        # Add custom response headers if configured
        if (!empty($this->api_config["custom_headers"])) {
            foreach ($this->api_config["custom_headers"] as $name=>$value) {
                header("$name: $value");
            }
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
                ref_id: "VIEW_COULD_NOT_DECODE_JSON",
                code: 406
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
    public function build_view() {
        # Get the fully qualified and non qualified class names for this object
        $fq_class_name = get_class($this);
        $nq_class_name = (new \ReflectionClass($this))->getShortName();

        # Specify the PHP code to write to the endpoints index.php file
        $code = "<?php\nrequire_once('api/views/".$nq_class_name.".inc');\n(new ".$fq_class_name."())->listen();\n";

        # Create directories and files corresponding with class
        if (!is_null($this->url)) {
            mkdir("/usr/local/www".$this->url, 0755, true);
            file_put_contents(
                "/usr/local/www".$this->url."/index.php",
                $code
            );
        }

        # Print success output if file now exists, otherwise output error and exit on non-zero code
        if (!is_null($this->url) and is_file("/usr/local/www".$this->url."/index.php")) {
            echo "Building ".$fq_class_name." at URL \"".$this->url."\"... done.".PHP_EOL;
        } else {
            echo "Building ".$fq_class_name." at URL \"".$this->url."\"... failed.".PHP_EOL;
            exit(1);
        }
    }
}
