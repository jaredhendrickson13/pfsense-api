<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many OpenVPNServerConnectionStatus Model objects at
 * /api/v2/status/openvpn/server/connections.
 */
class StatusOpenVPNServerConnectionsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/openvpn/server/connections';
        $this->model_name = 'OpenVPNServerConnectionStatus';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
