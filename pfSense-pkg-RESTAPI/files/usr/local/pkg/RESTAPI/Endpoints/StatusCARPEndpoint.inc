<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the CARP Model object at
 * /api/v2/status/carp.
 */
class StatusCARPEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/carp';
        $this->model_name = 'CARP';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
