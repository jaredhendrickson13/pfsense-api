<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many PortForward Model objects at /api/v2/firewall/nat/port_forwards.
 */
class FirewallNATPortForwardsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/nat/port_forwards';
        $this->model_name = 'PortForward';
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
