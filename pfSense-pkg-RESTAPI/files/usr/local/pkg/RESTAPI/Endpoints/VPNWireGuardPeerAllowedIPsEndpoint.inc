<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many WireGuardPeerAllowedIP Model objects at
 * /api/v2/vpn/wireguard/peer/allowed_ips.
 */
class VPNWireGuardPeerAllowedIPsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/wireguard/peer/allowed_ips';
        $this->model_name = 'WireGuardPeerAllowedIP';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
