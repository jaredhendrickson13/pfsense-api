<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single WireGuardTunnelAddress Model object at
 * /api/v2/vpn/wireguard/tunnel/address.
 */
class VPNWireGuardTunnelAddressEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/wireguard/tunnel/address';
        $this->model_name = 'WireGuardTunnelAddress';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
