<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemAPIJWT extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/api/jwt";
        $this->model_name = "APIJWT";
        $this->enforce_auth_mode = "local";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["POST"];
        $this->post_privileges = ["page-all", "page-system-api-jwt"];
        $this->delete_privileges = ["page-all", "page-system-api-jwt"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
