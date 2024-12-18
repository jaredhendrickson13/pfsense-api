<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple InterfaceLAGG Model objects at
 * /api/v2/interface/laggs.
 */
class InterfaceLAGGsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/interface/laggs';
        $this->model_name = 'InterfaceLAGG';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
