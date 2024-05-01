<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular OutboundNATMapping Model objects at 
 * /api/v2/firewall/nat/outbound/mapping.
 */
class FirewallNATOutboundMappingEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/nat/outbound/mapping';
        $this->model_name = 'OutboundNATMapping';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
