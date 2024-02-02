<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class DiagnosticsHaltSystemEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/halt_system';
        $this->model_name = 'SystemHalt';
        $this->request_method_options = ['POST'];
        $this->post_help_text = 'Initiates a System Halt.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
