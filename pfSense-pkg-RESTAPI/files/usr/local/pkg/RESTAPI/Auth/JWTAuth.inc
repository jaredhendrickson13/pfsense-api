<?php

namespace RESTAPI\Auth;

use RESTAPI\Core\Auth;
use RESTAPI\Models\RESTAPIJWT;

class JWTAuth extends Auth {
    /**
     * @var string|null $verbose_name The human-readable name for this Auth class.
     */
    public ?string $verbose_name = 'JWT';

    /**
     * @var array $security_scheme The OpenAPI security scheme for this Auth class.
     */
    public array $security_scheme = ['type' => 'http', 'scheme' => 'bearer', 'bearerFormat' => 'JWT'];

    /**
     * Obtains the submitted JWT from the client and attempts to decode the JWT using this host's server key.
     * @return bool Returns true if the JWT was successfully decoded, returns false otherwise.
     */
    public function _authenticate(): bool {
        # Decode the JWT using the bearer token provided by the client
        $decoded_jwt = RESTAPIJWT::decode($this->get_auth_key(identifier: 'bearer'));

        # Check that the JWT from our Authorization header is valid
        if ($decoded_jwt) {
            # Set the username of the authenticating client embedded in the decoded JWT data payload.
            $this->username = $decoded_jwt['data'];
            return true;
        }

        return false;
    }
}
