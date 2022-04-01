<?php
//   Copyright 2022 Jared Hendrickson
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
    public $query_excludes;

    # Set class contructor defaults
    public function __construct() {
        $this->url = null;
        $this->query_excludes = [];
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

    # Listen for HTTP requests and call the corresponding method
    public function listen() {
        $pkg_config = APITools\get_api_config()[1];

        # Before responding, ensure the request method is allowed
        if ($_SERVER["REQUEST_METHOD"] === "GET") {
            $resp = (new APIQuery($this->get(), $this->query_excludes))->query();
        }
        elseif ($_SERVER["REQUEST_METHOD"] === "POST") {
            $resp = $this->post();
        }
        elseif ($_SERVER["REQUEST_METHOD"] === "PUT") {
            $resp = $this->put();
        }
        elseif ($_SERVER["REQUEST_METHOD"] === "DELETE") {
            $resp = $this->delete();
        }
        # Only allow OPTIONS requests if the allow options setting is checked
        elseif ($_SERVER["REQUEST_METHOD"] === "OPTIONS" and isset($pkg_config["allow_options"])) {
            $resp = APIResponse\get(0);
        }
        # Method is not allowed when no match, set error response
        else {
            $resp = APIResponse\get(2);
        }

        # Add custom response headers if configured
        if (!empty($pkg_config["custom_headers"])) {
            foreach ($pkg_config["custom_headers"] as $name=>$value) {
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
        # Variables
        $content_type = ($_SERVER["HTTP_ACCEPT"] !== "*/*") ? $_SERVER["HTTP_ACCEPT"] : $_SERVER["HTTP_CONTENT_TYPE"];

        # Encode response as JSON if content type is application/json
        if ($content_type === "application/json") {
            header("Content-Type: application/json", true);
            return json_encode($data);
        }
        # Encode response as YAML if content type is application/yaml
        elseif ($content_type === "application/yaml") {
            header("Content-Type: application/yaml", true);
            return yaml_encode($data);
        }
        # Otherwise, just default to JSON
        else {
            header("Content-Type: application/json", true);
            return json_encode($data);
        }
    }
}
