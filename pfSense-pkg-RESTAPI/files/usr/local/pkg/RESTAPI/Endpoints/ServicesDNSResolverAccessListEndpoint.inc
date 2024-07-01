<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular DNSResolverAccessList Model object at
 * /api/v2/services/dns_resolver/access_list.
 */
class ServicesDNSResolverAccessListEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/access_list';
        $this->model_name = 'DNSResolverAccessList';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
