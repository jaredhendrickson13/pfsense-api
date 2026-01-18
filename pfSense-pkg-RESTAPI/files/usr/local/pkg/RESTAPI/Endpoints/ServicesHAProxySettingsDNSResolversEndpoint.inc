<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many HAProxyDNSResolver Model objects at
 * /api/v2/services/haproxy/settings/dns_resolvers.
 */
class ServicesHAProxySettingsDNSResolversEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/settings/dns_resolvers';
        $this->model_name = 'HAProxyDNSResolver';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
