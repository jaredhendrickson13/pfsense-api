<?php
require_once("api/framework/APITools.inc");


# Creates an object capable of verifying authentication and authorization based on the configuration
class APIAuth {
    private $api_config;
    private $request;
    public $auth_mode;
    public $req_privs;
    public $username;
    public $privs;
    public $is_authenticated;
    public $is_authorized;

    # Create our method constructor
    public function __construct($req_privs){
        $this->api_config = APITools\get_api_config()[1];
        $this->auth_mode = $this->api_config["authmode"];
        $this->request = APITools\get_request_data();
        $this->req_privs = $req_privs;
        $this->privs = [];
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
        if (APITools\authenticate_token($this->request["client-id"], $this->request["client-id"]) === true) {
            $this->username = pack("H*", $this->request["client-id"]);
            // Ensure user is not disabled
            if (APITools\is_user_disabled($this->request["client-id"]) === false) {
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
        // Local variables
        $authorized = false;
        $client_config =& getUserEntry($this->username);;
        $this->privs = get_user_privileges($client_config);
        $read_only = array_key_exists("readonly", $this->api_config);

        # If no require privileges were given, assume call is always authorized
        if (!empty($this->req_privs)) {
            // Ensure our API is not read only OR if it is read only, only authorize the request if it is a GET request
            if ($read_only === false or ($read_only === true and $_SERVER['REQUEST_METHOD'] === "GET")) {
                // Loop through each of our req privs and ensure the client has them, also check if access is read only
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