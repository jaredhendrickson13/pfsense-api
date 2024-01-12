<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class VPNIPsecPhase2Encryption extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/vpn/ipsec/phase2/encryption";
        $this->model_name = "IPsecPhase2Encryption";
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
