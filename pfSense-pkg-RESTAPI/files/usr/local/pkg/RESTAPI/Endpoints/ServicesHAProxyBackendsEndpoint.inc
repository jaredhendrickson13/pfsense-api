<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple HAProxyBackend Model objects at
 * /api/v2/services/haproxy/backends.
 */
class ServicesHAProxyBackendsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/backends';
        $this->model_name = 'HAProxyBackend';
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
