<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many TrafficShaperQueue Model objects at
 * /api/v2/firewall/traffic_shaper/queues.
 */
class FirewallTrafficShaperQueuesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/traffic_shaper/queues';
        $this->model_name = 'TrafficShaperQueue';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
