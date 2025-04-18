<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple InterfaceGRE Model objects at
 * /api/v2/interface/gres.
 */
class InterfaceGREsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/interface/gres';
        $this->model_name = 'InterfaceGRE';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
