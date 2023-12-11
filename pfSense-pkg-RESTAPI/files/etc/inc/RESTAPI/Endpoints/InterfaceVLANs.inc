<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class InterfaceVLANs extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/RESTAPI/v2/interface/vlans";
        $this->model_name = "InterfaceVLAN";
        $this->many = true;
        $this->request_method_options = ["GET"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
