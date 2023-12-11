<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallStatesSize extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/firewall/states/size";
        $this->model_name = "FirewallStatesSize";
        $this->request_method_options = ["GET", "PATCH"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
