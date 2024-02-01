<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class DiagnosticsARPTableEndpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/diagnostics/arp_table";
        $this->model_name = "ARPTable";
        $this->many = true;
        $this->request_method_options = ["GET"];
        
        # Construct the parent Endpoint object
        parent::__construct();
    }
}
