<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many HAProxyBackendErrorFile Model objects at
 * /api/v2/services/haproxy/backend/errorfiles.
 */
class ServicesHAProxyBackendErrorFilesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/backend/errorfiles';
        $this->model_name = 'HAProxyBackendErrorFile';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
