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

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIQuery.inc");

# Allow endpoints to access any API model class
foreach(glob("/etc/inc/api/models/*.inc") as $model) {
    require_once($model);
}

class APIEndpoint {
    public $url;
    public $errors;
    public $client;
    public $api_config;
    public $query_excludes;
    public $request_data;
    public $request_method;
    public $requires_auth;
    public $set_auth_mode;
    public $privileges;
    public $retain_read_mode;
    public $ignore_ifs;
    public $ignore_enabled;
    public $method_options;
    public $get_privileges;
    public $post_privileges;
    public $put_privileges;
    public $delete_privileges;

    # Set class constructor defaults
    public function __construct() {
        # Prevent PHP warnings in API responses
        error_reporting(E_ERROR);

        $this->url = null;
        $this->errors = [];
        $this->api_config = APITools\get_api_config()[1];
        $this->query_excludes = [];
        $this->request_data = APITools\get_request_data();
        $this->request_method = $_SERVER["REQUEST_METHOD"];
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

    # Model to run when endpoint receives a GET request
    protected function get() {
        return APIResponse\get(2);
    }

    # Model to run when endpoint receives a POST request
    protected function post() {
        return APIResponse\get(2);
    }

    # Model to run when endpoint receives a PUT request
    protected function put() {
        return APIResponse\get(2);
    }

    # Model to run when endpoint receives a DELETE request
    protected function delete() {
        return APIResponse\get(2);
    }

    /**
     * Checks the required privileges based on the request method.
     * @return array: an array of privileges required for the API call
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

    /**
     * Checks if the HTTP request method is allowed by this endpoint
     */
    private function check_request_method() {
        # Allow OPTIONS requests if configured
        if ($this->request_method === "OPTIONS" and isset($this->api_config["allow_options"])) {
            return;
        }

        # Set an error if the requested HTTP method is not an allowed option
        if (!in_array($this->request_method, $this->method_options)) {
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

        # Checks that request data was properly received.
        $this->check_request_method();
        $this->check_request_data();

        # Checks client authentication and authorization if required
        if ($this->requires_auth) {
            $this->check_auth();
        }

        # Proceed if no errors were encountered
        if (!$this->errors) {
            # Before responding, ensure the request method is allowed
            if ($this->request_method === "GET") {
                $resp = (new APIQuery($this->get(), $this->query_excludes))->query();
            }
            elseif ($this->request_method === "POST") {
                $resp = $this->post();
            }
            elseif ($this->request_method === "PUT") {
                $resp = $this->put();
            }
            elseif ($this->request_method === "DELETE") {
                $resp = $this->delete();
            }
            elseif ($this->request_method === "OPTIONS") {
                $resp = APIResponse\get(0);
                header("access-control-allow-methods: ".implode(", ", $this->method_options));
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
}
