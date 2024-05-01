<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the RoutingApply Model object at 
 * /api/v2/routing/apply.
 */
class RoutingApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/apply';
        $this->model_name = 'RoutingApply';
        $this->request_method_options = ['GET', 'POST'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
