<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many DNSForwarderHostOverrideAlias Model objects at
 * /api/v2/services/dns_forwarder/host_override/aliases.
 */
class ServicesDNSForwarderHostOverrideAliasesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_forwarder/host_override/aliases';
        $this->model_name = 'DNSForwarderHostOverrideAlias';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
