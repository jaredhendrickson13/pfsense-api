<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular DNSForwarderHostOverride Model object at
 * /api/v2/services/dns_forwarder/host_override.
 */
class ServicesDNSForwarderHostOverrideEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_forwarder/host_override';
        $this->model_name = 'DNSForwarderHostOverride';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
