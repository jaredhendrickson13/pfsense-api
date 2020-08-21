<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");

class APIAccessToken extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->set_auth_mode = "local";
        $this->methods = ["POST"];
        $this->validators = [
            $this->validateAuthMode()
        ];
    }

    # Validate our API configurations auth mode (must be JWT)
    private function validateAuthMode() {
        $api_config = APITools\get_api_config()[1];

        # Add error if our auth mode is invalid
        if ($api_config["authmode"] !== "jwt") {
            return APIResponse\get(9);
        }
    }

    # Override action subclass to create a JWT and return it to the user
    public function action() {
        $jwt = APITools\create_jwt($this->client->username);
        return APIResponse\get(0, ["token" => $jwt]);
    }
}