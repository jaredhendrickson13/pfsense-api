<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class RoutingApply extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/routing/apply";
        $this->model_name = "RoutingApply";
        $this->tag = "ROUTING";
        $this->request_method_options = ["GET", "POST"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
