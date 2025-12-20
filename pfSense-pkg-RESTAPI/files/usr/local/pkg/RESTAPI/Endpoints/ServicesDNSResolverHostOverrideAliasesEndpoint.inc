<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many DNSResolverHostOverrideAlias Model objects at
 * /api/v2/services/dns_resolver/host_override/aliases.
 */
class ServicesDNSResolverHostOverrideAliasesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/host_override/aliases';
        $this->model_name = 'DNSResolverHostOverrideAlias';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
