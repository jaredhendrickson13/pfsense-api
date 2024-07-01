<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many FirewallLog Models at /api/v2/status/logs/firewall.
 */
class StatusLogsFirewallEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/logs/firewall';
        $this->model_name = 'FirewallLog';
        $this->many = true;
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
