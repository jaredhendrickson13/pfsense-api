<?php
//   Copyright 2020 Jared Hendrickson
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
        if ($_SERVER["REQUEST_METHOD"] === "GET") {
            $resp = (new APIQuery($this->get(), $this->query_excludes))->query();
        } elseif ($_SERVER["REQUEST_METHOD"] === "POST") {
            $resp = $this->post();
        } elseif ($_SERVER["REQUEST_METHOD"] === "PUT") {
            $resp = $this->put();
        } elseif ($_SERVER["REQUEST_METHOD"] === "DELETE") {
            $resp = $this->delete();
        } else {
            $resp = APIResponse\get(2);
        }

        # Format the HTTP response as JSON and set response code
        header("Content-Type: application/json", true);
        header("Referer: no-referrer");
        http_response_code($resp["code"]);
        echo json_encode($resp) . PHP_EOL;
        exit();
    }
}