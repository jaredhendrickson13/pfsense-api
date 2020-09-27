<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

class APIAccessTokenCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->set_auth_mode = "local";
    }

    # Validate our API configurations auth mode (must be JWT)
    public function validate_payload() {
        $api_config = APITools\get_api_config()[1];

        # Add error if our auth mode is invalid
        if ($api_config["authmode"] !== "jwt") {
            $this->errors[] = APIResponse\get(9);
        }
    }

    # Override action subclass to create a JWT and return it to the user after successful validation
    public function action() {
        $jwt = APITools\create_jwt($this->client->username);
        return APIResponse\get(0, ["token" => $jwt]);
    }
}