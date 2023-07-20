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
namespace APIFramework;

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIQuery.inc");

use APITools;
use APIAuth;
use APIResponse;
use ReflectionMethod, ReflectionException;

# Allow endpoints to access any API model class
foreach(glob("/etc/inc/api/models/*.inc") as $model) {
    require_once($model);
}

class APIView {
    public $url;
    protected $model_name;
    protected $model;
    protected $errors;
    protected $client;
    public $api_config;
    public $query_excludes;
    public $request_data;
    public $request_method;
    public $requires_auth;
    public $set_auth_mode;
    public $retain_read_mode;
    public $ignore_ifs;
    public $ignore_enabled;
    public $method_options;
    public $get_privileges;
    public $post_privileges;
    public $put_privileges;
    public $delete_privileges;

    public function __construct() {
        # Prevent PHP warnings in API responses
        error_reporting(E_ERROR);

        $this->url = null;
        $this->model_name = "APIModel";
        $this->errors = [];
        $this->api_config = APITools\get_api_config()[1];
        $this->query_excludes = [];
        $this->request_data = APIView::get_request_data();
        $this->request_method = strtoupper($_SERVER["REQUEST_METHOD"]);
        $this->requires_auth = true;
        $this->retain_read_mode = true;
        $this->ignore_ifs = false;
        $this->ignore_enabled = false;
        $this->method_options = [];
        $this->get_privileges = [];
        $this->post_privileges = [];
        $this->put_privileges = [];
        $this->delete_privileges = [];
    }

    /**
     * Checks which HTTP methods are supported by checking which APModel methods were overridden.
     * @return void No value returned but $this->method_options will be pop
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
            $this->method_options[] = "OPTIONS";
        }

        # Check if the corresponding model method is configured for each HTTP method
        foreach ($method_map as $http_method=>$class_method) {
            # Try to create a reflection object to use when comparing child class to the parent class
            try {
                $reflection = new ReflectionMethod($this->model, $class_method);
            }
            catch (ReflectionException $ref_ex) {
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
    private function get_required_privs() {
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

    private function check_auth() {
        $read_only = (isset($this->api_config["readonly"]) and $this->retain_read_mode) ? true : false;
        $this->client = new APIAuth($this->get_required_privs(), $this->set_auth_mode, $read_only);
        if ($this->requires_auth === true) {
            if (!$this->client->is_authenticated) {
                $this->errors[] = APIResponse\get(3);
            }
            if (!$this->client->is_authorized) {
                $this->errors[] = APIResponse\get(4);
            }
        }
    }

    # Check if the API is enabled before answering calls, if not, redirect to wc login
    private function check_enable() {
        $api_config = APITools\get_api_config()[1];
        if (!isset($api_config["enable"]) and !$this->ignore_enabled) {
            header("Location: /");
            die();
        }
    }

    # Check if server IP is allowed to answer API calls. Redirects to login if not
    private function check_server_ip() {
        $allow_ifs = $this->api_config["allowed_interfaces"];
        $whitelist = explode(",", $allow_ifs);

        # Check if our server IP is in our whitelist
        foreach ($whitelist as $wif) {
            $if_info = get_interface_info($wif);
            # Check if our server IP is a valid if address, VIP, localhost, or any
            if ($_SERVER["SERVER_ADDR"] === $if_info["ipaddr"] or APITools\is_ip_vip($_SERVER["SERVER_ADDR"], $wif)) {
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
        $this->errors[] = APIResponse\get(6);
    }

    private function check_request_method() {
        # Check if the requested HTTP method is a supported HTTP method.
        if (!in_array($this->request_method, $this->get_method_options())) {
            $this->errors[] = APIResponse\get(2);
        }
    }

    # Check if our requested content-type is supported and parsed data correctly, sets error if not
    private function check_request_data() {
        if ($this->request_data === false) {
            $this->errors[] = APIResponse\get(11);
        }
    }

    # Listen for HTTP requests and call the corresponding method
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
                $resp = APIResponse\get(0);
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

    # Encodes the API response data to the requested or most relevant content type
    public function content_type_encode($data) {
        # TODO: in future if additional response Content-Types need to be supported we can add them here
        # TODO: and control the requested content-type using the Accept HTTP header
        # Encode response as JSON if content type is application/json
        header("Content-Type: application/json", true);
        return json_encode($data);
    }

    # Checks our content type header and parses the content accordingly
    public static function get_request_data() {
        # TODO: x-www-form-urlencoded still attempts to use json if no parameters were passed in. This was intentionally done to
        # TODO: support user scripts that didn't specify a static content type (before it was supported) remove this in a future
        # TODO: release. It is preferred that content uses a specified content type.
        $_SERVER["HTTP_CONTENT_TYPE"] = (empty($_SERVER["HTTP_CONTENT_TYPE"])) ? "application/x-www-form-urlencoded" : $_SERVER["HTTP_CONTENT_TYPE"];
        $content_types = [
            "application/json" => json_decode(file_get_contents('php://input'), true),
            "application/x-www-form-urlencoded" => (empty($_GET)) ? json_decode(file_get_contents('php://input'), true) : APIView::form_decode($_GET)
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

    # Decode query string into their appropriate data types
    public static function form_decode($array) {
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
        # Variables
        $class_name = get_class($this);

        # Specify the PHP code to write to the endpoints index.php file
        $code = "<?php\nrequire_once('api/views/".$class_name.".inc');\n(new APIViews\\".$class_name."())->listen();\n";

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
            echo "Building ".$class_name." view at URL \"".$this->url."\"... done.".PHP_EOL;
        } else {
            echo "Building ".$class_name." view at URL \"".$this->url."\"... failed.".PHP_EOL;
            exit(1);
        }
    }
}
