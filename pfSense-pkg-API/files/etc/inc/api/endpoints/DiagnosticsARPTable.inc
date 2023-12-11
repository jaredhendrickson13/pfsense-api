<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class DiagnosticsARPTable extends Endpoint
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
