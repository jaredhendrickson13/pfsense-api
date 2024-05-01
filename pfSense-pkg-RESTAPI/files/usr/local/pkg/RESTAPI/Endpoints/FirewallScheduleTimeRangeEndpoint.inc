<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular FirewallScheduleTimeRange Model objects at 
 * /api/v2/firewall/schedule/time_range.
 */
class FirewallScheduleTimeRangeEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/schedule/time_range';
        $this->model_name = 'FirewallScheduleTimeRange';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
