<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class InterfaceBridge extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/RESTAPI/v2/interface/bridge";
        $this->model_name = "InterfaceBridge";
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
