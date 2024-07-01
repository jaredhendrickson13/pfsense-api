<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular DHCPServerBackend Model object at
 * /api/v2/services/dhcp_server/backend.
 */
class ServicesDHCPServerEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server';
        $this->model_name = 'DHCPServer';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
