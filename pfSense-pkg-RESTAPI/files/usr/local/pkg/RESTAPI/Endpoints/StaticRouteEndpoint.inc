<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class StaticRouteEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/routing/static_route';
        $this->model_name = 'StaticRoute';
        $this->tag = 'ROUTING';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
