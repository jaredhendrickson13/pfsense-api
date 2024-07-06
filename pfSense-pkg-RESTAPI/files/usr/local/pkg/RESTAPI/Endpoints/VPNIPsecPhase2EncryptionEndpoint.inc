<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single IPsecPhase2Encryption Model objects at
 * /api/v2/vpn/ipsec/phase2/encryption.
 */
class VPNIPsecPhase2EncryptionEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/ipsec/phase2/encryption';
        $this->model_name = 'IPsecPhase2Encryption';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
