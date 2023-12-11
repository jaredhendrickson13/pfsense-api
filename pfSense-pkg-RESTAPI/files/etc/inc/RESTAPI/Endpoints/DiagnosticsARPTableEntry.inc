<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class DiagnosticsARPTableEntry extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/RESTAPI/v2/diagnostics/arp_table/entry";
        $this->model_name = "ARPTable";
        $this->request_method_options = ["GET", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
