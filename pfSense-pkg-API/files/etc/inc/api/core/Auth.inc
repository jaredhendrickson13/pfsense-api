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

namespace API\Core;

require_once("api/auto_loader.inc");

use API;
use Exception;
use Firebase\JWT\JWT;
use Firebase\JWT\Key;

/**
 * Creates object that performs authentication and authorization for API endpoints.
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
        $this->api_config = API\Models\APISettings::get_api_config()["config"];

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
        # Authenticate against the local user database
        if (authenticate_user($username, $password)) {
            $this->username = $username;
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
        $decoded_jwt = $this->decode_jwt($jwt);

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
        foreach ($this->get_existing_keys() as $key_from_config) {
            # Hash the incoming key according to the algorithm of this stored key
            $hash = hash($key_from_config["algo"], $key);

            # Authentication is success when incoming client key matches the hashed key stored in config
            if ($hash === $key_from_config["hash"]) {
                $this->username = $key_from_config["username"];
                unset($_SESSION["Username"]);
                $_SESSION["Username"] = $key_from_config["username"];
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
     * Initializes the JWT server key if none is present. Optionally allows the existing JWT server key to be rotated.
     * @param bool $rotate Force the JWT server key to be rotated.
     * @throws Exception When insufficient source of randomness is not found for `random_bytes`.
     */
    public static function init_jwt_server_key(bool $rotate = false) {
        # Obtain the API package's ID and configuration
        $pkg_id = API\Models\APISettings::get_api_config()["id"];
        $api_config = API\Models\APISettings::get_api_config()["config"];

        # Create a new server key if one is not set
        if (empty($api_config["server_key"]) or $rotate === true) {
            config_set_path("installedpackages/package/$pkg_id/conf/server_key", bin2hex(random_bytes(32)));
            write_config("API server key created");
        }
    }

    /**
     * @param mixed $username The username the JWT is to be issued for.
     * @return string The JWT string clients will use for JWT authentication.
     * @throws Exception When the JWT server key could not be created.
     */
    public static function generate_jwt(string $username) : string {
        # Initialize the JWT server key in case it is not present
        self::init_jwt_server_key();

        # Pull the API configuration from config
        $api_config = API\Models\APISettings::get_api_config()["config"];

        # Assign the JWT payload values. Assign the configured JWT expiration from the API configuration
        $payload = array(
            "iss" => config_get_path("system/hostname"),
            "aud" => config_get_path("system/hostname"),
            "exp" => time() + $api_config["jwt_exp"],
            "nbf" => time(),
            "data" => $username
        );

        # Return the encoded JWT
        return JWT::encode($payload, $api_config["server_key"], "HS256");
    }


    /**
     * Decodes a provided JWT string and extracts its attributes.
     * @param string $jwt The incoming JWT string to decode.
     * @return array An array of decoded JWT attributes. If the JWT could not be decoded, this array will be empty.
     */
    public static function decode_jwt(string $jwt) : array {
        # Obtain the JWT server key from config
        $jwt_server_key = API\Models\APISettings::get_api_config()["config"]["server_key"];

        # Attempt to decode the JWT, if decoding fails return empty array.
        try {
            return (array) JWT::decode($jwt, new Key($jwt_server_key, "HS256"));
        }
        catch (Exception) {
            return [];
        }
    }


    /**
     * Generates a new persistent API key.
     * @param string $username The username to assign this API key to.
     * @return string The real (unhashed)
     * @throws Exception
     */
    public static function generate_key(string $username) : string {
        # Variables
        $pkg_id = API\Models\APISettings::get_api_config()["id"];
        $api_config = API\Models\APISettings::get_api_config()["config"];
        $hash_algo = $api_config["key_hash_algo"];
        $key_bit_strength = $api_config["key_length_bytes"];
        $key_new = bin2hex(random_bytes(intval($key_bit_strength)));
        $hash = hash($hash_algo, $key_new);

        # Initialize keys config and add this key
        $api_config["keys"] = !is_array($api_config["keys"]) ? array("key" => []) : $api_config["keys"];
        $api_config["keys"]["key"][] = ["username" => $username, "hash" => $hash, "algo" => $hash_algo];

        # Write the new key to config
        config_set_path("installedpackages/package/$pkg_id/conf", $api_config);
        write_config(sprintf(gettext(" Generated API key")));
        return $key_new;
    }

    /**
     * Obtain the API keys that belong to a provided username.
     * @param string $username The username to obtain API keys for.
     * @return array An array of API keys that belong to the $username
     */
    public static function get_existing_keys(string $username = "", $all = true) : array {
        # Variables
        $api_config = API\Models\APISettings::get_api_config()["config"];
        $user_keys = [];

        # Only loop through if we have configured keys
        if (is_array($api_config["keys"]) and is_array($api_config["keys"]["key"])) {
            foreach ($api_config["keys"]["key"] as $id=>$key) {
                if ($key["username"] === $username or $all) {
                    $user_keys[$id] = $key;
                }
            }
        }

        return $user_keys;
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
