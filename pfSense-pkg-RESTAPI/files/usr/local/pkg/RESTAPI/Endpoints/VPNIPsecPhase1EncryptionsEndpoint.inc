<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many IPsecPhase1Encryption Model objects at
 * /api/v2/vpn/ipsec/phase1/encryptions.
 */
class VPNIPsecPhase1EncryptionsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/ipsec/phase1/encryptions';
        $this->model_name = 'IPsecPhase1Encryption';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
