<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class FirewallAliasesEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/aliases';
        $this->model_name = 'FirewallAlias';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
