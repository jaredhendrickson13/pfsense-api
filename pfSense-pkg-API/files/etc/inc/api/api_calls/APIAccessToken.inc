<?php
require_once("api/APITools.inc");
require_once("api/APIResponse.inc");
require_once("api/APIAuth.inc");

class APIAccessToken {
    private $client;
    private $methods;
    private $req_privs;
    public $errors;
    public $valid_data;

    # Create our method constructor
    public function __construct() {
        $this->req_privs = [];
        $this->methods = ["GET"];
        $this->client = new APIAuth($this->req_privs);
        $this->errors = [];
        $this->valid_data = [];
    }

    # Validate our API configurations auth mode (must be JWT)
    private function validateAuthMode() {
        $api_config = APITools\get_api_config()[1];

        # Add error if our auth mode is invalid
        if ($api_config["authmode"] !== "jwt") {
            $this->errors[] = APIResponse\get(9);
        }
    }

    # Validate our request
    public function validate($validate_auth=true, $validate_http_method=true) {
        # Validate authentication
        if ($validate_auth === true) {
            # Add error if user is not authenticated
            if (!$this->client->is_authenticated) {
                $this->errors[] = APIResponse\get(3);
            }
            # Add error if user is not authorized
            if (!$this->client->is_authorized) {
                $this->errors[] = APIResponse\get(4);
            }
        }

        # Validate HTTP method
        if ($validate_http_method === true) {

        }

        # Run our field/conditional validators
        $this->validateAuthMode();

        # Check if we have errors in our error array
        if (count($this->errors) === 0) {
            return true;
        } else {
            return false;
        }
    }

    # Run our call. This method will return an assoc array containing the API response results
    public function call() {
        # Check if our request is valid
        if ($this->validate()) {
            $jwt = api_create_jwt($this->client->username);
            return ApiResponse\get(0, [["token" => $jwt]]);
        } else {
            return $this->errors[0];
        }
    }

    # Listen for client requests. This method should executed on the API endpoint.
    public function listen() {
        # RUN API CALL
        $resp = $this->call();
        http_response_code($resp["code"]);
        echo json_encode($resp) . PHP_EOL;
        exit();
    }
}