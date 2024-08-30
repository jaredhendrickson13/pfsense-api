<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple StaticRoute Model objects at
 * /api/v2/routing/static_routes.
 */
class RoutingStaticRoutesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/static_routes';
        $this->many = true;
        $this->model_name = 'StaticRoute';
        $this->tag = 'ROUTING';
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
