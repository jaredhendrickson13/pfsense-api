<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many TrafficShaperLimiterBandwidth Model objects at
 * /api/v2/firewall/traffic_shaper/limiter/bandwidths.
 */
class FirewallTrafficShaperLimiterBandwidthsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/traffic_shaper/limiter/bandwidths';
        $this->model_name = 'TrafficShaperLimiterBandwidth';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
