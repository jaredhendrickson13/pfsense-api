<?php
require_once("api/framework/APITools.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/framework/APIAuth.inc");

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
        $this->methods = ["GET", "POST"];
        $this->privileges = ["page-all"];
        $this->client = new APIAuth($this->privileges);
        $this->requires_auth = true;
        $this->validators = [];
        $this->initial_data = APITools\get_request_data();
        $this->validated_data = [];
        $this->errors = [];

    }

    private function check_authentication() {
        if ($this->requires_auth === true) {
            if (!$this->client->is_authenticated) {
                $this->errors[] = APIResponse\get(3);
            }
        }
    }

    private function check_authorization() {
        if (!$this->client->is_authorized) {
            $this->errors[] = APIResponse\get(4);
        }
    }

    private function check_method() {
        if (!in_array($_SERVER["REQUEST_METHOD"], $this->methods)) {
            $this->errors[] = APIResponse\get(2);
        }
    }

    public function action() {
        # This function is intended to be overridden by an API model extended class
        # Any configuration writes, system configurations, etc should be added when overriding this base class
        # If this class is not overridden a 500 unexpected error is returned
        return APIResponse\get(1);
    }

    public function validate() {
        $this->check_method();
        if ($this->requires_auth) {
            $this->check_authentication();
            $this->check_authorization();
        }
        $this->errors = array_merge($this->errors, $this->validators);


        if (count($this->errors) === 0) {
            return true;
        } else {
            return false;
        }
    }

    public function call() {
        if ($this->validate()) {
            return $this->action();
        } else {
            return $this->errors[0];
        }
    }

    public function listen() {
        $resp = $this->call();
        http_response_code($resp["code"]);
        echo json_encode($resp) . PHP_EOL;
        exit();
    }

}