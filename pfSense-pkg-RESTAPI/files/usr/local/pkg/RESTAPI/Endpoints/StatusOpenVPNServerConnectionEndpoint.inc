<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNServerConnectionStatus Model objects at
 * /api/v2/status/openvpn/server/connection.
 */
class StatusOpenVPNServerConnectionEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/openvpn/server/connection';
        $this->model_name = 'OpenVPNServerConnectionStatus';
        $this->request_method_options = ['GET', 'DELETE'];

        # Set help text
        $this->delete_help_text = 'Kills an existing OpenVPN Server Connection';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
