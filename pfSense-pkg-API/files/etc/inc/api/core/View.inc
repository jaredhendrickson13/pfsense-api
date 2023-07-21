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
    public mixed $request_data = [];
    public string $request_method;
    public bool $requires_auth = true;
    public string $set_auth_mode = "";
    public bool $retain_read_mode = true;
    public bool $ignore_ifs = false;
    public bool $ignore_enabled = false;
    public array $get_privileges = [];
    public array $post_privileges = [];
    public array $put_privileges = [];
    public array $delete_privileges = [];

    /**
     * Sets values whenever the View object is crated.
     */
    public function __construct() {
        # Prevent PHP warnings in API responses.
        error_reporting(E_ERROR);

        # Fetch the current API configuration whenever the object is first created.
        $this->api_config = API\Core\Tools\get_api_config()[1];

        # Fetch request data and method whenever the object is first created.
        $this->request_data = $this->get_request_data();
        $this->request_method = strtoupper($_SERVER["REQUEST_METHOD"]);
    }

    /**
     * Checks which HTTP methods are supported by checking which APModel methods were overridden.
     * @return array An array of supported HTTP methods
     */
    private function get_method_options() : array {
        # Variables
        $method_options = [];
        $method_map = [
            "GET" => "_read",
            "POST" => "_create",
            "PUT" => "_update",
            "DELETE" => "_delete"
        ];

        # Allow OPTIONS requests if configured
        if (isset($this->api_config["allow_options"])) {
            $method_options[] = "OPTIONS";
        }

        # Check if the corresponding model method is configured for each HTTP method
        foreach ($method_map as $http_method=>$class_method) {
            # Try to create a reflection object to use when comparing child class to the parent class
            try {
                $reflection = new \ReflectionMethod($this->model, $class_method);
            }
            catch (\ReflectionException $ref_ex) {
                continue;
            }

            # If the child class has overridden the method, add this HTTP method as an allowed option.
            if ($reflection->getDeclaringClass()->getName() === get_class($this->model)) {
                $method_options[] = $http_method;
            }
        }

        return $method_options;
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
        elseif ($this->request_method === "PUT") {
            return $this->put_privileges;
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
        $read_only = (isset($this->api_config["readonly"]) and $this->retain_read_mode) ? true : false;
        $this->client = new API\Core\Auth($this->get_required_privs(), $this->set_auth_mode, $read_only);
        if ($this->requires_auth === true) {
            if (!$this->client->is_authenticated) {
                $this->errors[] = API\Core\Response\get(3);
            }
            if (!$this->client->is_authorized) {
                $this->errors[] = API\Core\Response\get(4);
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
            die();
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
            } elseif ($this->ignore_ifs === true) {
                return;
            }
        }

        # Return 444 response if we did not find a previous match
        $this->errors[] = API\Core\Response\get(6);
    }

    /**
     * Check if the HTTP method requested by the API client is supported by this view.
     */
    private function check_request_method() {
        # Check if the requested HTTP method is a supported HTTP method.
        if (!in_array($this->request_method, $this->get_method_options())) {
            $this->errors[] = API\Core\Response\get(2);
        }
    }

    /**
     * Checks if the API client's request data was successfully received and parsed by the API.
     */
    private function check_request_data() {
        if ($this->request_data === false) {
            $this->errors[] = API\Core\Response\get(11);
        }
    }

    /**
     * Listens for incoming API calls from API clients and checks if the API call is allowed.
     */
    public function listen() {
        # Ensure API is enabled and the receiving interface is allowed to answer API calls.
        $this->check_enable();
        $this->check_server_ip();

        # Checks client authentication and authorization if required
        if ($this->requires_auth) {
            $this->check_auth();
        }

        # Initialize the assigned model
        $this->model_name = "APIModels\\".$this->model_name;
        $this->model = new $this->model_name($this->request_data, $this->client);

        # Checks that request data was properly received.
        $this->check_request_method();
        $this->check_request_data();

        # Proceed if no errors were encountered
        if (!$this->errors) {
            # Run the associated model method based on the incoming HTTP method
            if ($this->request_method === "GET") {
                $resp = $this->model->read();
            }
            elseif ($this->request_method === "POST") {
                $resp = $this->model->create();
            }
            elseif ($this->request_method === "PUT") {
                $resp = $this->model->update();
            }
            elseif ($this->request_method === "DELETE") {
                $resp = $this->model->delete();
            }
            elseif ($this->request_method === "OPTIONS") {
                $resp = API\Core\Response\get(0);
                header("access-control-allow-methods: ".implode(", ", $this->get_method_options()));
            }
        }
        # Respond with endpoint error if one was encountered
        else {
            $resp = $this->errors[0];
        }

        # Add custom response headers if configured
        if (!empty($this->api_config["custom_headers"])) {
            foreach ($this->api_config["custom_headers"] as $name=>$value) {
                header(strval($name).": ".strval($value));
            }
        }

        # Add API required response headers, these will override any custom headers
        header("Referer: no-referrer");

        # Format the HTTP response as JSON and set response code
        http_response_code($resp["code"]);
        echo $this->content_type_encode($resp) . PHP_EOL;
        session_destroy();
        exit();
    }

    /**
     * Encodes the API response according to the Accept header received by the API client.
     * @param array $data The API response data to encode in the requested format.
     * @return false|string The encoded API response or `false` if the data could not be encoded.
     */
    public function content_type_encode(array $data) : bool|string {
        # TODO: in future if additional response Content-Types need to be supported we can add them here
        # TODO: and control the requested content-type using the Accept HTTP header
        # Encode response as JSON if content type is application/json
        header("Content-Type: application/json", true);
        return json_encode($data);
    }

    /**
     * Parses the API client's request data using the Content-Type specified by the API client.
     * @return false|mixed the API client's parsed request data or `false` if the data could not be parsed.
     */
    public static function get_request_data() {
        # TODO: x-www-form-urlencoded still attempts to use json if no parameters were passed in. This was intentionally done to
        # TODO: support user scripts that didn't specify a static content type (before it was supported) remove this in a future
        # TODO: release. It is preferred that content uses a specified content type.
        $_SERVER["HTTP_CONTENT_TYPE"] = (empty($_SERVER["HTTP_CONTENT_TYPE"])) ? "application/x-www-form-urlencoded" : $_SERVER["HTTP_CONTENT_TYPE"];
        $content_types = [
            "application/json" => json_decode(file_get_contents('php://input'), true),
            "application/x-www-form-urlencoded" => (empty($_GET)) ? json_decode(file_get_contents('php://input'), true) : View::form_decode($_GET)
        ];

        # Check if content type is supported, if so return corresponding parsed request data
        if (array_key_exists($_SERVER["HTTP_CONTENT_TYPE"], $content_types)) {
            return $content_types[$_SERVER["HTTP_CONTENT_TYPE"]];
        }
        # Return false if the content type is unknown or unsupported
        else {
            return false;
        }
    }

    /**
     * Applies primitive type assumption to application/x-www-form-urlencoded request data.
     * @param array $array The form data array to decode.
     * @return array The parsed API client's request data
     */
    public static function form_decode($array) : array {
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
