<?php
//   Copyright 2024 Jared Hendrickson
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

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Auth\BasicAuth;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\User;
use RESTAPI\Responses\AuthenticationError;
use RESTAPI\Responses\ForbiddenError;
use RESTAPI\Responses\ServerError;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Defines object that performs authentication and authorization for REST API Endpoints and obtains details about the
 * authenticating client. This class is intended to be extended by child classes that define specific authentication
 * methods.
 */
class Auth {
    use BaseTraits;

    /**
     * @var array $api_settings The current REST API settings config. This contains the settings internal values.
     */
    private array $api_settings;

    /**
     * @var array $security_scheme Sets the OpenAPI securityScheme definition for this Auth class. This value should
     * be the innermost array of the securityScheme and not include the scheme name.
     * @link SecurityScheme https://swagger.io/docs/specification/authentication/
     * @example ["type" => "apiKey", "in" => "header", "name" => "Authorization"]
     */
    public array $security_scheme = [];

    /**
     * @var array $required_privileges The list of privileges that allow authorization.
     */
    public array $required_privileges;

    /**
     * @var array $client_privileges The list of privileges the remote client currently has assigned.
     */
    public array $client_privileges = [];

    /**
     * @var string|null The username of the authenticated remote client.
     */
    public ?string $username = null;

    /**
     * @var string $ip_address The IP address of the remote client.
     */
    public string $ip_address = '';

    /**
     * @var bool $is_authenticated Indicates whether the remote client is currently authenticated.
     */
    public bool $is_authenticated = false;

    /**
     * @var bool $is_authenticated Indicates whether the remote client is currently authorized.
     */
    public bool $is_authorized = false;

    /**
     * @var string|null $verbose_name The human-readable name for this Auth class.
     */
    public ?string $verbose_name = null;

    /**
     * Creates an Auth object which can be used to authenticate an external client requesting API access.
     * @param array $required_privileges An array of required pfSense privileges to check during authorization.
     */
    public function __construct(array $required_privileges = []) {
        # Obtain the internal API package configuration
        $this->api_settings = RESTAPISettings::get_pkg_config();

        # Assign required privileges
        $this->required_privileges = $required_privileges;
        $this->ip_address = $_SERVER['REMOTE_ADDR'] ?: 'unknown IP';

        # Use the class shortname as the default verbose name if none was set by a child class
        $this->verbose_name = $this->verbose_name ?: $this->get_class_shortname();
    }

    /**
     * Provides additional logic whenever authentication is successful. This is intended to be overridden by a
     * child Auth class when it needs something specific to happen after a client successfully authenticates.
     */
    protected function on_successful_auth(): void {
    }

    /**
     * Provides additional logic whenever authentication has failed. This is intended to be overridden by a
     * child Auth class when it needs something specific to happen after a client fails to authenticate.
     */
    protected function on_failed_auth(): void {
    }

    /**
     * Defines the steps required for users to authenticate wit this authentication method. This method must be
     * overridden by a child Auth class. This method MUST return the username of the authenticating client if auth
     * is successful, and null if it is not. NOTE: This method MUST set $this->username.
     * @return bool Returns true if authentication was successful, returns false otherwise.
     * @throws ServerError When this method is not overridden by a child class.
     */
    public function _authenticate(): bool {
        $class = get_called_class();
        throw new ServerError(
            message: "Auth class `$class` must override `_authenticate()` method.",
            response_id: 'AUTH_CLASS_WITHOUT_OVERRIDDEN_AUTHENTICATE_METHOD',
        );
    }

    /**
     * Perform authentication using the current authentication method.
     * @return bool `true` if authentication was successful, `false` if authentication was unsuccessful.
     */
    final public function authenticate(): bool {
        # Assume user is no longer authenticated or authorized.
        $this->is_authenticated = false;
        $this->is_authorized = false;
        $this->username = null;
        unset($_SESSION['Username']);

        # Start the user session if authentication was successful and the user was not disabled.
        if ($this->_authenticate() and $this->is_user_enabled($this->username)) {
            $_SESSION['Username'] = $this->username;
            $this->is_authenticated = true;
            $this->on_successful_auth();
        }
        # Otherwise, fail authentication.
        else {
            $this->is_authenticated = false;
            $this->on_failed_auth();
        }

        # Log this authentication attempt and return the authentication result
        $this->log_authentication();

        return $this->is_authenticated;
    }

    /**
     * Checks if the client is authorized to make this request.
     * @return bool `true` if authorization was successful, `false` if authorization was unsuccessful.
     */
    public function authorize(): bool {
        # Variables
        $is_not_authorized = false;
        $this->client_privileges = get_user_privileges(getUserEntry($this->username));

        # This client is not authorized if the client does not have at least one of the required privileges
        if ($this->required_privileges and !array_intersect($this->required_privileges, $this->client_privileges)) {
            $is_not_authorized = true;
        }

        # Return the authorization results
        $this->is_authorized = !$is_not_authorized;
        return $this->is_authorized;
    }

    /**
     * Checks if the remote client has requested this Auth class for authentication. By default, this method will
     * simply try to authenticate the remote client using the called Auth class. This can also be overridden by
     * child Auth classes to define custom logic to determine if the client is trying to use the called Auth
     * class.
     */
    public function is_requested(): bool {
        return $this->_authenticate();
    }

    /**
     * Checks if a given username is an existing user that is not disabled
     * @param string $username The username to check.
     * @return bool `true` if the user is disabled, `false` if it is not.
     */
    public static function is_user_enabled(string $username): bool {
        return User::query(name: $username, disabled: false)->exists();
    }

    /**
     * Logs API authentication according to the current API configuration.
     */
    private function log_authentication(): void {
        # Variables
        $username = $this->username ?: 'unknown';
        $ip_address = $this->ip_address;

        # Log authentication attempts if enabled
        if ($this->api_settings['login_protection'] === 'enabled') {
            # Log failed authentication
            if (!$this->is_authenticated) {
                # This log entry is required for Login Protection to work, do not change the log text.
                log_auth(gettext("webConfigurator authentication error for user '{$username}' from: {$ip_address}"));
            }
            # Log successful authentication if the API is configured to do so. Disabled by default to avoid log spam.
            elseif ($this->api_settings['log_successful_auth'] === 'enabled') {
                log_auth(gettext("Successful login for user '{$username}' from: {$ip_address} (Local Database)"));
            }
        }
    }

    /**
     * Obtains the key with a specified identifier (i.e. bearer) from the client via the authorization header.
     * @param String $identifier The key identifier to look for in the authorization header (i.e. 'bearer', 'apiKey').
     * Note: This identifier is case-insensitive.
     * @returns String A string containing the submitted bearer token from the client. If a key with the matching
     * $identifier is not found, an empty string will be returned.
     */
    public static function get_auth_key(string $identifier): string {
        # Obtain the key from the authorization header.
        $auth_header = explode(' ', $_SERVER['HTTP_AUTHORIZATION']);
        $token_type = $auth_header[0];
        $secret = $auth_header[1] ?: '';

        # Ensure the value was passed in with the same identifier.
        if (strtolower($token_type) === strtolower($identifier)) {
            return $secret;
        }

        # Return an empty string if there were any problems obtaining the secret
        return '';
    }

    /**
     * Throws an error if a given Auth class shortname is not an Auth child class in the \RESTAPI\Auth\ namespace.
     * @param string $class_name The name of the Auth class to check for.
     * @throws ServerError When $class_name is not an existing Auth child class in the \RESTAPI\Auth\ namespace
     */
    public static function ensure_class_exists(string $class_name): void {
        # Variables
        $namespace = '\\RESTAPI\\Auth\\';

        # Do not allow this $auth_class if it is not an existing Auth class
        if (!in_array($namespace . $class_name, get_classes_from_namespace($namespace))) {
            throw new ServerError(
                message: "Auth class `$class_name` does not exist.",
                response_id: 'AUTH_CLASS_DOES_NOT_EXIST',
            );
        }
    }

    /**
     * Attempts authentication for each allowed Auth class. Authentication AND authorization must pass using at least
     * one of the allowed Auth classes for authentication to be considered successful. In the event that all Auth
     * class methods fail, an error will be thrown detailing the failure.
     * @param array $auth_classes Specifies Auth classes that should be included in this multi auth attempt.
     * This must be an array of existing Auth class short names. If no value is assigned, the Auth methods configured
     * in the REST API settings will be assumed.
     * @param array $required_privileges An array of pfSense privileges that are pre-authorized. The client must hold at
     * least one of these privileges to be considered authorized.
     * @return Auth The Auth object which succeeded authentication and authorization. This object will contain the
     * details about the authenticated client.
     * @throws AuthenticationError When no allowed Auth classes had successful authentication.
     * @throws ForbiddenError When authentication was successful for an Auth class, but the client failed to be
     * authorized due to missing privileges.
     */
    final public static function attempt_multi_auth(array $auth_classes = [], array $required_privileges = []): Auth {
        # Variables
        $auth_namespace = '\\RESTAPI\\Auth\\';
        $matched_auth = null;
        $api_settings = new RESTAPISettings();
        $auth_classes = $auth_classes ?: $api_settings->auth_methods->value;

        # Loop through each allowed Auth class and perform preliminary authentication
        foreach ($auth_classes as $auth_class) {
            # Do not allow this $auth_class if it is not an existing Auth class
            self::ensure_class_exists($auth_class);

            # Create a new object for this Auth class
            $auth_class = $auth_namespace . $auth_class;
            $auth = new $auth_class(required_privileges: $required_privileges);

            # If this Auth class is being requested by the remote client, set the matched auth and break the loop
            if ($auth->is_requested()) {
                $matched_auth = $auth;
                break;
            }
        }

        # Log an authentication error if no Auth class could be determined
        if (!$matched_auth) {
            $auth = new Auth();
            $auth->log_authentication();
        }

        # Throw an authentication error if no preliminary auth passed or authentication for the identified class fails
        if (!$matched_auth or !$matched_auth->authenticate()) {
            throw new AuthenticationError(message: 'Authentication failed.', response_id: 'AUTH_AUTHENTICATION_FAILED');
        }
        # Throw an authorization error if authorization for the identified class fails
        elseif (!$matched_auth->authorize()) {
            throw new ForbiddenError(
                message: 'Authorization failed. You do not have sufficient privileges to access this resource.',
                response_id: 'AUTH_AUTHORIZATION_FAILED',
            );
        }

        return $matched_auth;
    }
}
