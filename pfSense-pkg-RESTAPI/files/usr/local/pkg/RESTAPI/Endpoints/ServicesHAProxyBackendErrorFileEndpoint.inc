<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular HAProxyBackendErrorFile Model object at 
 * /api/v2/services/haproxy/backend/error_file.
 */
class ServicesHAProxyBackendErrorFileEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/backend/error_file';
        $this->model_name = 'HAProxyBackendErrorFile';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
