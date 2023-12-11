<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemRESTAPIJWT extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/RESTAPI/jwt";
        $this->model_name = "RESTAPIJWT";
        $this->enforce_auth_mode = "local";
        $this->request_method_options = ["POST"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
