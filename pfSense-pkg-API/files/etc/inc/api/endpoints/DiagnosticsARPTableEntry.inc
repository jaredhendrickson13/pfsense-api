<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class DiagnosticsARPTableEntry extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/diagnostics/arp_table/entry";
        $this->model_name = "ARPTable";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "DELETE"];
        $this->get_privileges = ["page-all", "page-diagnostics-arptable"];
        $this->delete_privileges = ["page-all", "page-diagnostics-arptable"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
