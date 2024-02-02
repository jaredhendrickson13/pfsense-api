<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class RoutingGatewaysEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/routing/gateways';
        $this->model_name = 'RoutingGateway';
        $this->many = true;
        $this->tag = 'ROUTING';
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
