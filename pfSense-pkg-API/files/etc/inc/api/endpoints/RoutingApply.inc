<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

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
