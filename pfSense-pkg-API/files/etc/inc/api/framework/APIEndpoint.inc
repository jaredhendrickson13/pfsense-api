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

# Allow endpoints to access any API model class
foreach(glob("/etc/inc/api/models/*.inc") as $model) {
    require_once($model);
}

class APIEndpoint {
    public $url;

    # Set class contructor defaults
    public function __construct() {
        $this->url = null;
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

    # Limit GET results using search queries
    private function query_get_data($resp) {
        $excluded_keys = ["client-token", "client-id"];
        # Check if response is eligible for search querying
        if ($resp["return"] === 0) {
            $new_resp = $resp;
            $new_resp["data"] = [];
            # Loop through each request and ensure it is an array, if so, check for query matches
            foreach ($resp["data"] as $id=>$data) {
                if (is_array($data)) {
                    $is_query = false;
                    # Loop through each payload value and check for matches
                    foreach (APITools\get_request_data() as $key=>$value) {
                        # Ensure key is not excluded
                        if (!in_array($key, $excluded_keys)) {
                            $is_query = true;
                            if (array_key_exists($key, $data) and $data[$key] === $value) {
                                $match = true;
                            } else {
                                $match = false;
                                break;
                            }
                        }
                    }

                    # If this entry matched our query, add it to our new response.
                    if ($match === true) {
                        $new_resp["data"][$id] = $data;
                    }

                } else {
                    return $resp;
                }
            }

            # Only return our query results if user passed in query parameters
            if ($is_query) {
                return $new_resp;
            } else {
                return $resp;
            }

        } else {
            return $resp;
        }
    }

    # Listen for HTTP requests and call the corresponding method
    public function listen() {
        if ($_SERVER["REQUEST_METHOD"] === "GET") {
            $resp = $this->query_get_data($this->get());
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