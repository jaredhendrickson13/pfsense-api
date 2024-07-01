<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular TrafficShaperLimiterQueue Model objects at
 * /api/v2/firewall/traffic_shaper/limiter/queue.
 */
class FirewallTrafficShaperLimiterQueueEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/traffic_shaper/limiter/queue';
        $this->model_name = 'TrafficShaperLimiterQueue';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
