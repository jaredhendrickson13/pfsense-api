<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemRESTAPIVersion extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/restapi/version";
        $this->model_name = "RESTAPIVersion";
        $this->request_method_options = ["GET"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
