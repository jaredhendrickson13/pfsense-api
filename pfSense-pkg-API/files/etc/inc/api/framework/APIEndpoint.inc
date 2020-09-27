<?php
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

    # Listen for HTTP requests and call the corresponding method
    public function listen() {
        if ($_SERVER["REQUEST_METHOD"] === "GET") {
            $resp = $this->get();
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