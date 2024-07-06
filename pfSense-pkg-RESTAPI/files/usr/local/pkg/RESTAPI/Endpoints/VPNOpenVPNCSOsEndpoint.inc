<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many OpenVPNClientSpecificOverride Model objects at
 * /api/v2/vpn/openvpn/csos.
 */
class VPNOpenVPNCSOsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/openvpn/csos';
        $this->model_name = 'OpenVPNClientSpecificOverride';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
