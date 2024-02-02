<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class ServicesDNSResolverHostOverridesEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/host_overrides';
        $this->model_name = 'DNSResolverHostOverride';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
