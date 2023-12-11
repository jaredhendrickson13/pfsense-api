<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesDHCPServer extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/services/dhcp_server";
        $this->model_name = "DHCPServer";
        $this->request_method_options = ["GET", "PATCH"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
