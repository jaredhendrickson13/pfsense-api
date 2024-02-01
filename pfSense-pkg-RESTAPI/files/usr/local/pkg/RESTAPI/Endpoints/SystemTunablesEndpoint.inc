<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemTunablesEndpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/tunables";
        $this->model_name = "SystemTunable";
        $this->many = true;
        $this->request_method_options = ["GET", "PUT"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
