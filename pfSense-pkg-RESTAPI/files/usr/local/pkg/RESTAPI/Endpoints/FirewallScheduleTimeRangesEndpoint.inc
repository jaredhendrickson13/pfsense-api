<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple FirewallScheduleTimeRange Model objects at
 * /api/v2/firewall/schedule/time_ranges.
 */
class FirewallScheduleTimeRangesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/schedule/time_ranges';
        $this->model_name = 'FirewallScheduleTimeRange';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
