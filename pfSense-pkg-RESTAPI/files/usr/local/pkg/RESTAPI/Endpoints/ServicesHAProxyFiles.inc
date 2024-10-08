<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple HAProxyFile Model objects at
 * /api/v2/services/haproxy/file.
 */
class ServicesHAProxyFiles extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/files';
        $this->model_name = 'HAProxyFile';
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
