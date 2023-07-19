<?php
//   Copyright 2023 Jared Hendrickson
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
    public function __construct($initial_data=[], $client=null) {
        parent::__construct($initial_data, $client);
    }

    # Validate our API configurations auth mode (must be JWT)
    public function validate_payload() {
        $api_config = APITools\get_api_config()[1];

        # Return a JWT if JWT is the configured auth mode
        if ($api_config["authmode"] === "jwt") {
            # Ensure the JWT server key exists and create the requested JWT
            APITools\create_jwt_server_key();
            $this->validated_data= ["token"=>APITools\create_jwt($this->client->username)];
        }
        # Return the API client ID and API client token if API Token is the configured auth mode
        elseif ($api_config["authmode"] === "token") {
            $this->validated_data= [
                "client-id"=>bin2hex($this->client->username),
                "client-token"=>APITools\generate_token($this->client->username)
            ];
        }
        # Otherwise, return an error because the configured auth mode does not support tokens
        else {
            $this->errors[] = APIResponse\get(9);
        }
    }

    # Override action subclass to create a JWT and return it to the user after successful validation
    public function action() {
        # Create and return the JWT to the client
        return APIResponse\get(0, $this->validated_data);
    }
}
