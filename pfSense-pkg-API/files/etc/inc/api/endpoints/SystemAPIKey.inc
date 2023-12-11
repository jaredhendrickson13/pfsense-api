<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemAPIKey extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/api/key";
        $this->model_name = "APIKey";
        $this->enforce_auth_mode = "local";
        $this->request_method_options = ["POST", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
