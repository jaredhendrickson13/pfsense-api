<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many TrafficShaperLimiter Model objects at
 * /api/v2/firewall/traffic_shaper/limiters.
 */
class FirewallTrafficShaperLimitersEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/traffic_shaper/limiters';
        $this->model_name = 'TrafficShaperLimiter';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
