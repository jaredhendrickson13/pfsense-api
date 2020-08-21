<?php
require_once("api/framework/APITools.inc");
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/framework/APIAuth.inc");

class APIAccessToken extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["GET"];
        $this->validators = ["validateAuthMode"];
    }

    # Validate our API configurations auth mode (must be JWT)
    private function validateAuthMode() {
        $api_config = APITools\get_api_config()[1];

        # Add error if our auth mode is invalid
        if ($api_config["authmode"] !== "jwt") {
            $this->errors[] = APIResponse\get(9);
        }
    }

    # Override action subclass to create a JWT and return it to the user
    public function action() {
        $jwt = api_create_jwt($this->client->username);
        return APIResponse\get(0, ["token" => $jwt]);
    }
}