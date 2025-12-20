<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many DNSResolverAccessListNetwork Model objects at
 * /api/v2/services/dns_resolver/access_list/networks.
 */
class ServicesDNSResolverAccessListNetworksEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/access_list/networks';
        $this->model_name = 'DNSResolverAccessListNetwork';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
