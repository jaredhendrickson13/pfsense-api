<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the DNSResolverSettings Model object at
 * /api/v2/services/dns_resolver/settings.
 */
class ServicesDNSResolverSettingsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/settings';
        $this->model_name = 'DNSResolverSettings';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
