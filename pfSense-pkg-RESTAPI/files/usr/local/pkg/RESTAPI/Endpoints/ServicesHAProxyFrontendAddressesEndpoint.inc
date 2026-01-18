<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many HAProxyFrontendAddress Model objects at
 * /api/v2/services/haproxy/frontend/addresses.
 */
class ServicesHAProxyFrontendAddressesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/frontend/addresses';
        $this->model_name = 'HAProxyFrontendAddress';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
