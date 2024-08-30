<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many DNSForwarderHostOverride Model objects at
 * /api/v2/services/dns_forwarder/host_override.
 */
class ServicesDNSForwarderHostOverridesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_forwarder/host_overrides';
        $this->model_name = 'DNSForwarderHostOverride';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
