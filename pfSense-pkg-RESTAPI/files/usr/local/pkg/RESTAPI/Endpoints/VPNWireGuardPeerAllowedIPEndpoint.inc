<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single WireGuardPeerAllowedIP Model object at 
 * /api/v2/vpn/wireguard/peer/allowed_ip.
 */
class VPNWireGuardPeerAllowedIPEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/wireguard/peer/allowed_ip';
        $this->model_name = 'WireGuardPeerAllowedIP';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
