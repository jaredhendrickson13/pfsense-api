<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemAPIJWT extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/api/jwt";
        $this->model_name = "APIJWT";
        $this->enforce_auth_mode = "local";
        $this->request_method_options = ["POST"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
