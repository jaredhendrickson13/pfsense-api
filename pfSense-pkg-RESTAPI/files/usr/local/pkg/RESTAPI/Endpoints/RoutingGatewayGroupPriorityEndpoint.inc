<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single RoutingGatewayGroupPriority Model object at
 * /api/v2/routing/gateway/group/priority.
 */
class RoutingGatewayGroupPriorityEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/gateway/group/priority';
        $this->model_name = 'RoutingGatewayGroupPriority';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
