<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular DNSResolverHostOverrideAlias Model object at
 * /api/v2/services/dns_resolver/host_override/alias.
 */
class ServicesDNSResolverHostOverrideAliasEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/host_override/alias';
        $this->model_name = 'DNSResolverHostOverrideAlias';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
