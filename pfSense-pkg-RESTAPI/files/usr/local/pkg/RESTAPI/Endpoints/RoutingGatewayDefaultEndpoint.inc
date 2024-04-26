<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class RoutingGatewayDefaultEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/routing/gateway/default';
        $this->model_name = 'DefaultGateway';
        $this->tag = 'ROUTING';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
