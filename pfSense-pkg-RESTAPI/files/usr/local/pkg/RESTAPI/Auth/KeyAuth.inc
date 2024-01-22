<?php

namespace RESTAPI\Auth;

use RESTAPI\Core\Auth;
use RESTAPI\Models\RESTAPIKey;

class KeyAuth extends Auth
{
    /**
     * Performs REST API key authentication and obtains the username of the user who owns the provided key.
     * @return bool Returns true if match for this client's key found a match stored in config, returns false
     * otherwise.
     */
    public function _authenticate(): bool
    {
        # Obtain the API key from the client via Authorization header with 'key' identifier.
        $key = $this->get_auth_key(identifier: "key");

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