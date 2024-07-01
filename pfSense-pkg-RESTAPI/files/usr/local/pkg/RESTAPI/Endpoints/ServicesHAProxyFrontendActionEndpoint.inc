<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular HAProxyFrontendAction Model object at
 * /api/v2/services/haproxy/frontend/action.
 */
class ServicesHAProxyFrontendActionEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/frontend/action';
        $this->model_name = 'HAProxyFrontendAction';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
