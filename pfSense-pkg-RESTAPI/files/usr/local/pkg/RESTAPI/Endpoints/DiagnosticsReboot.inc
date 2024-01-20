<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class DiagnosticsReboot extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/diagnostics/reboot";
        $this->model_name = "SystemReboot";
        $this->request_method_options = ["POST"];
        $this->post_help_text = "Initiates a System Reboot.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
