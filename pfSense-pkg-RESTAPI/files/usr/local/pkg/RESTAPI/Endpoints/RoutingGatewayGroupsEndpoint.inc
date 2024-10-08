<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple RoutingGatewayGroup Model objects at
 * /api/v2/routing/gateway/groups.
 */
class RoutingGatewayGroupsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/gateway/groups';
        $this->model_name = 'RoutingGatewayGroup';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
