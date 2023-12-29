<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class VPNIPsecPhase2s extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/vpn/ipsec/phase2s";
        $this->model_name = "IPsecPhase2";
        $this->many = true;
        $this->request_method_options = ["GET", "PUT"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
