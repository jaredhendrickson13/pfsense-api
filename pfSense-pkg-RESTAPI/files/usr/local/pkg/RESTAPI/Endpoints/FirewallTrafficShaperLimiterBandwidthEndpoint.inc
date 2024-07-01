<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular TrafficShaperLimiterBandwidth Model objects at
 * /api/v2/firewall/traffic_shaper/limiter/bandwidth.
 */
class FirewallTrafficShaperLimiterBandwidthEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/traffic_shaper/limiter/bandwidth';
        $this->model_name = 'TrafficShaperLimiterBandwidth';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
