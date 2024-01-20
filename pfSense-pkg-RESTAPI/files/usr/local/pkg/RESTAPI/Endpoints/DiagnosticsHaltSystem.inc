<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class DiagnosticsHaltSystem extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/diagnostics/halt_system";
        $this->model_name = "SystemHalt";
        $this->enforce_auth_mode = "local";
        $this->request_method_options = ["POST"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
