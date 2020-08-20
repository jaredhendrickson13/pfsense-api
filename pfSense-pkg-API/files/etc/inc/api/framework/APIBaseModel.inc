<?php
require_once("api/APITools.inc");
require_once("api/APIResponse.inc");
require_once("api/APIAuth.inc");

class APIBaseModel {
    public $client;
    public $privileges;
    public $validators;
    public $initial_data;
    public $validated_data;
    public $errors;
    public $methods;
    public $requires_auth;


    public function __construct() {
        $this->methods = ["POST"];
        $this->privileges = ["pages-all"];
        $this->client = new APIAuth($this->privileges);
        $this->requires_auth = true;
        $this->validators = [];
        $this->initial_data = APITools\get_request_data();
        $this->validated_data = [];
        $this->errors = [];

    }

    # Validate our request
    public function validate() {
        # Validate authentication
        if ($this->requires_auth === true) {
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
        if (!in_array($_SERVER["REQUEST_METHOD"], $this->methods)) {

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

}