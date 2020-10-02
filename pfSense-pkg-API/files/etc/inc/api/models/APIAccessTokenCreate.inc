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
require_once("api/framework/APIResponse.inc");

class APIAccessTokenCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->set_auth_mode = "local";
        $this->bypass_read_mode = false;
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