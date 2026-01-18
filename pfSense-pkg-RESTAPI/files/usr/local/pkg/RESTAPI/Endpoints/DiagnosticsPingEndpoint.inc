<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for running ping diagnostics at /api/v2/diagnostics/ping.
 */
class DiagnosticsPingEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/ping';
        $this->model_name = 'Ping';
        $this->request_method_options = ['POST'];
        $this->post_help_text = 'Run a ping command from pfSense to a specified host.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
