<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the SystemVersion Model object at
 * /api/v2/system/version.
 */
class SystemVersionEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/version';
        $this->model_name = 'SystemVersion';
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
