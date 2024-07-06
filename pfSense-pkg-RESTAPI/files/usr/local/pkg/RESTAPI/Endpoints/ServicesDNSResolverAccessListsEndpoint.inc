<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple DNSResolverAccessList Model objects at
 * /api/v2/services/dns_resolver/access_lists.
 */
class ServicesDNSResolverAccessListsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/access_lists';
        $this->many = true;
        $this->model_name = 'DNSResolverAccessList';
        $this->request_method_options = ['GET', 'PUT'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
