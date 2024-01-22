<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class AuthKey extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/auth/key";
        $this->model_name = "RESTAPIKey";
        $this->auth_methods = ["BasicAuth"];
        $this->request_method_options = ["POST", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
