<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many DNSResolverHostOverride Model objects at
 * /api/v2/services/dns_resolver/host_override.
 */
class ServicesDNSResolverDomainOverridesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/domain_overrides';
        $this->model_name = 'DNSResolverDomainOverride';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
