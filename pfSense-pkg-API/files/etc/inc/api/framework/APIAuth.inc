<?php
//   Copyright 2021 Jared Hendrickson
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

require_once("api/framework/APITools.inc");

# Creates an object capable of verifying authentication and authorization based on the configuration
class APIAuth {
    private $api_config;
    private $request;
    public $auth_mode;
    public $read_mode;
    public $req_privs;
    public $username;
    public $privs;
    public $ip_address;
    public $is_authenticated;
    public $is_authorized;

    # Create our method constructor
    public function __construct($req_privs, $enforce_auth_mode=null, $read_mode=null){
        $this->api_config = APITools\get_api_config()[1];
        $this->auth_mode = (is_null($enforce_auth_mode)) ? $this->api_config["authmode"] : $enforce_auth_mode;
        $this->read_mode = (is_null($read_mode)) ? array_key_exists("readonly", $this->api_config) : $read_mode;
        $this->request = APITools\get_request_data();
        $this->req_privs = $req_privs;
        $this->privs = [];
        $this->ip_address = $_SERVER["REMOTE_ADDR"];
        $this->is_authenticated = $this->authenticate();
        $this->is_authorized = $this->authorize();
    }

    # AUTHENTICATION #
    # Attempts to authenticate using local database authentication
    private function authenticate_local_database() {
        $this->username = $this->request["client-id"];
        // Authenticate against local database
        if (authenticate_user($this->username, $this->request["client-token"])) {
            // Ensure user is not disabled
            if (APITools\is_user_disabled($this->username) === false) {
                unset($_SESSION["Username"]);
                $_SESSION["Username"] = $this->username;
                return true;
            }
        }
        return false;
    }

    # Attempts to authenticate using JWT authentication
    private function authenticate_jwt() {
        $auth_header = explode(" ", $_SERVER["HTTP_AUTHORIZATION"]);
        $token_type = $auth_header[0];
        $token = $auth_header[1];
        $decoded_jwt = APITools\decode_jwt($token);

        // Check that the JWT from our Authorization header is valid
        if ($token_type === "Bearer" and $decoded_jwt !== false) {
            $this->username = $decoded_jwt["data"];
            // Ensure user is not disabled
            if (APITools\is_user_disabled($this->username) === false) {
                unset($_SESSION["Username"]);
                $_SESSION["Username"] = $this->username;
                return true;
            }
        }
        return false;
    }

    # Attempts to authenticate using API token authentication
    private function authenticate_token() {
        # Check if the API token was passed in via authorization header
        # TODO: in a future release make this the only way to pass in token credentials
        if (!empty($_SERVER["HTTP_AUTHORIZATION"])) {
            $this->request["client-id"] = explode(" ", $_SERVER["HTTP_AUTHORIZATION"])[0];
            $this->request["client-token"] = explode(" ", $_SERVER["HTTP_AUTHORIZATION"])[1];
        }

        if (APITools\authenticate_token($this->request["client-id"], $this->request["client-token"]) === true) {
            $this->username = pack("H*", $this->request["client-id"]);
            # Ensure user is not disabled
            if (APITools\is_user_disabled($this->username) === false) {
                unset($_SESSION["Username"]);
                $_SESSION["Username"] = $this->username;
                return true;
            }
        }
        return false;
    }

    # Chooses correct auth method based on configured auth mode. Returns bool.
    public function authenticate() {
        # Attempt to authenticate
        if ($this->auth_mode === "local") {
            $resp = $this->authenticate_local_database();
        }
        elseif ($this->auth_mode === "jwt") {
            $resp = $this->authenticate_jwt();
        }
        elseif ($this->auth_mode === "token") {
            $resp = $this->authenticate_token();
        }
        else {
            $resp = false;
        }

        # Set our class is_authenticated attribute to our authentication resp and return the resp
        $this->is_authenticated = $resp;
        return $this->is_authenticated;
    }

    # AUTHORIZATION #
    # Checks if the current user has the necessary privileges to access this resource
    public function authorize() {
        # Local variables
        $authorized = false;
        $client_config =& getUserEntry($this->username);;
        $this->privs = get_user_privileges($client_config);

        # If no require privileges were given, assume call is always authorized
        if (!empty($this->req_privs)) {
            # If API is in readonly mode, only allow GET requests
            if (!$this->read_mode or ($this->read_mode and $_SERVER['REQUEST_METHOD'] === "GET")) {
                # Loop through each of our req privs and ensure the client has them, also check if access is read only
                foreach ($this->req_privs as &$priv) {
                    if (in_array($priv, $this->privs)) {
                        $authorized = true;
                        break;
                    }
                }
            }
        } else {
            $authorized = true;
        }

        # Set our class is_authorized attribute to our authorization resp and return the resp
        $this->is_authorized = $authorized;
        return $authorized;
    }
}