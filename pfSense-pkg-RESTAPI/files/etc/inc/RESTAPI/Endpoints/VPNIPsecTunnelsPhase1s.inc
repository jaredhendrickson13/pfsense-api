<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class VPNIPsecTunnelsPhase1s extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/vpn/ipsec/tunnels/phase1s";
        $this->model_name = "IPsecPhase1";
        $this->many = true;
        $this->request_method_options = ["GET", "PUT"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
