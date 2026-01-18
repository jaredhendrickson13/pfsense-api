<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many RoutingGatewayGroupPriority Model objects at
 * /api/v2/routing/gateway/group/priorities.
 */
class RoutingGatewayGroupPrioritiesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/gateway/group/priorities';
        $this->model_name = 'RoutingGatewayGroupPriority';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
