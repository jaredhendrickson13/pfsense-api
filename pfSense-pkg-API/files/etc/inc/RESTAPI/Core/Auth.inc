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

namespace RESTAPI\Core;

require_once("RESTAPI/autoloader.inc");

use RESTAPI;
use RESTAPI\Models\RESTAPIJWT;
use RESTAPI\Models\RESTAPIKey;
use Exception;
use Firebase\JWT\JWT;
use Firebase\JWT\Key;

/**
 * Creates object that performs authentication and authorization for API Endpoints.
 */
class Auth {
    private array $api_config;
    public string $mode;
    public bool $read_only;
    public array $required_privileges;
    public array $client_privileges = [];
    public string $username = "unknown user";
    public string $ip_address = "unknown IP";
    public bool $is_authenticated = false;
    public bool $is_authorized = false;

    /**
     * Creates an Auth object which can be used to authenticate an external client requesting API access.
     * @param array $required_privileges An array of required pfSense privileges to check during authorization.
     * @param string $mode Explicitly sets the API `auth_mode` to use during authentication.
     * Defaults to `auth_mode` API configuration value.
     * @param bool|null $read_only Explicitly set the API `read_mode` during authentication. Defaults to `readonly`
     * API configuration value.
     */
    public function __construct(array $required_privileges=[], string $mode="", bool|null $read_only=null){
        # Obtain the internal API package configuration
        $this->api_config = RESTAPI\Models\APISettings::get_api_config()["config"];

        # Allow the auth_mode and read_only settings to be overwritten, otherwise default to their configured values
        $this->mode = (!$mode) ? $this->api_config["auth_mode"] : $mode;
        $this->read_only = (is_null($read_only)) ? $this->api_config["read_only"] === "enabled" : $read_only;

        # Assign required privileges
        $this->required_privileges = $required_privileges;
        $this->ip_address = ($_SERVER["REMOTE_ADDR"]) ?: "unknown IP";
    }

    /**
     * Attempts to authenticate using local authentication.
     * @param string $username The username of the user authenticating.
     * @param string $password The password of the user authenticating.
     * @return bool `true` if authentication was successful, `false` if authentication was unsuccessful.
     */
    private function authenticate_local_database(string $username, string $password) : bool {
        $this->username = $username;

        # Authenticate against the local user database
        if (authenticate_user($username, $password)) {
            unset($_SESSION["Username"]);
            $_SESSION["Username"] = $username;
            return true;
        }

        return false;
    }

    /**
     * Attempts to authenticate using JWT authentication.
     * @param string $jwt The JWT encoded string to authenticate.
     * @return bool `true` if authentication was successful, `false` if authentication was unsuccessful.
     */
    private function authenticate_jwt(string $jwt) : bool {
        # Decode the JWT
        $decoded_jwt = RESTAPIJWT::decode($jwt);

        # Check that the JWT from our Authorization header is valid
        if ($decoded_jwt) {
            # Extract the owner's username from the JWT's payload data
            $this->username = $decoded_jwt["data"];
            unset($_SESSION["Username"]);
            $_SESSION["Username"] = $this->username;
            return true;
            }

        return false;
    }

    /**
     * Attempts to authenticate using API key authentication.
     * @return bool `true` if authentication was successful, `false` if authentication was unsuccessful.
     */
    private function authenticate_key(string $key) : bool {
        # Loop through the existing keys for this user and check the key hashes for a match
        foreach (RESTAPIKey::read_all()->model_objects as $api_key_obj) {
            if ($api_key_obj->authenticate($key)) {
                $this->username = $api_key_obj->username->value;
                unset($_SESSION["Username"]);
                $_SESSION["Username"] = $this->username;
                return true;
            }
        }

        return false;
    }

    /**
     * Run authentication using the current configured auth mode.
     * @param string $secret The secret password, token or key from the authenticating client.
     * @param string $username The username of the authenticating client. This only applies to `local` authentication.
     * @return bool `true` if authentication was successful, `false` if authentication was unsuccessful.
     */
    public function authenticate(string $secret, string $username = "") : bool {
        # Attempt to authenticate using the assigned auth mode
        $this->is_authenticated = match ($this->mode) {
            "local" => $this->authenticate_local_database(username: $username, password: $secret),
            "jwt" => $this->authenticate_jwt(jwt: $secret),
            "key" => $this->authenticate_key(key: $secret),
            default => false,
        };

        # Fail authentication if the user authenticating is disabled
        if ($this->is_user_disabled($this->username)) {
            $this->is_authenticated = false;
        }

        # Log this authentication attempt and return the authentication result
        $this->log_authentication();
        return $this->is_authenticated;
    }

    /**
     * Checks if the client is authorized to make this request.
     * @return bool `true` if authorization was successful, `false` if authorization was unsuccessful.
     */
    public function authorize() : bool {
        # Variables
        $is_not_authorized = false;
        $this->client_privileges = get_user_privileges(getUserEntry($this->username));

        # This client is not authorized if this `readonly` mode is enabled but this isn't a read request
        if ($this->read_only and $_SERVER['REQUEST_METHOD'] !== "GET") {
            $is_not_authorized= true;
        }

        # This client is not authorized if the client does not have at least one of the required privileges
        if ($this->required_privileges and !array_intersect($this->required_privileges, $this->client_privileges)) {
            $is_not_authorized= true;
        }

        # Return the authorization results
        $this->is_authorized = !$is_not_authorized;
        return $this->is_authorized;
    }

    /**
     * Checks if a given username is an existing user, but is disabled.
     * @param string $username The username to check.
     * @return bool `true` if the user is disabled, `false` if it is not.
     */
    private function is_user_disabled(string $username) : bool {
        $users = index_users();
        $user = $users[$username];
        if (config_path_enabled("system/user/{$user}", "disabled")) {
            return true;
        }
        return false;
    }

    /**
     * Logs API authentication according to the current API configuration.
     */
    private function log_authentication() {
        # Variables
        $username = ($this->username) ?: "unknown";
        $ip_address = $this->ip_address;

        # Log authentication attempts if enabled
        if ($this->api_config["login_protection"] === "enabled") {
            # Log failed authentication
            if (!$this->is_authenticated) {
                # This log entry is required for Login Protection to work, do not change the log text.
                log_auth(
                    gettext("webConfigurator authentication error for user '{$username}' from: {$ip_address}")
                );
            }
            # Log successful authentication if the API is configured to do so. Disabled by default to avoid log spam.
            elseif ($this->api_config["log_successful_auth"] === "enabled") {
                log_auth(
                    gettext("Successful login for user '{$username}' from: {$ip_address} (Local Database)")
                );
            }
        }
    }
}
