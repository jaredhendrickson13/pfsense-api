<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many FirewallRule Model objects at /api/v2/firewall/rules.
 */
class FirewallRulesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/rules';
        $this->model_name = 'FirewallRule';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
