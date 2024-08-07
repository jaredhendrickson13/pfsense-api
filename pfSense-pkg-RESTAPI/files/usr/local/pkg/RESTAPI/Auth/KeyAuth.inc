<?php

namespace RESTAPI\Auth;

use RESTAPI\Core\Auth;
use RESTAPI\Models\RESTAPIKey;

/**
 * Defines an Auth class that allows for API key authentication against the pfSense REST API. Key auth is better
 * suited to distribute to systems as they are revocable and will only allow API authentication; not webConfigurator
 * or SSH authentication (like the local database credentials).
 */
class KeyAuth extends Auth {
    /**
     * @var string|null $verbose_name The human-readable name for this Auth class.
     */
    public ?string $verbose_name = 'Key';

    /**
     * @var array $security_scheme The OpenAPI security scheme for this Auth class.
     */
    public array $security_scheme = ['type' => 'apiKey', 'in' => 'header', 'name' => 'x-api-key'];

    /**
     * Performs REST API key authentication and obtains the username of the user who owns the provided key.
     * @return bool Returns true if match for this client's key found a match stored in config, returns false
     * otherwise.
     */
    public function _authenticate(): bool {
        # Obtain the API key from the client via x-api-key header
        $key = $_SERVER['HTTP_X_API_KEY'] ?? '';

        # Loop through the existing keys for this user and check the key hashes for a match
        foreach (RESTAPIKey::read_all()->model_objects as $api_key_obj) {
            if ($api_key_obj->authenticate($key)) {
                $this->username = $api_key_obj->username->value;
                return true;
            }
        }

        return false;
    }
}
