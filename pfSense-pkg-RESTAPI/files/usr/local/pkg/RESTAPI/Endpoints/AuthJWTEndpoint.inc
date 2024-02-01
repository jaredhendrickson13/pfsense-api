<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class AuthJWTEndpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/auth/jwt";
        $this->model_name = "RESTAPIJWT";
        $this->auth_methods = ["BasicAuth"];
        $this->request_method_options = ["POST"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
