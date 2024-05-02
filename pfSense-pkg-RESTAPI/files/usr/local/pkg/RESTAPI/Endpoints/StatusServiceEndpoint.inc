<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a Service Model object at
 * /api/v2/status/service.
 */
class StatusServiceEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/service';
        $this->model_name = 'Service';
        $this->request_method_options = ['POST'];
        $this->post_help_text = 'Triggers a start, stop or restart action for an existing Service';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
