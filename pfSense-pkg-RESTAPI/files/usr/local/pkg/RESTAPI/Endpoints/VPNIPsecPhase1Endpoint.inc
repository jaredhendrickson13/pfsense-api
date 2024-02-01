<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class VPNIPsecPhase1Endpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/vpn/ipsec/phase1";
        $this->model_name = "IPsecPhase1";
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
