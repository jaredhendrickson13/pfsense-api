<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemConsoleEndpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/console";
        $this->model_name = "SystemConsole";
        $this->request_method_options = ["GET", "PATCH"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
