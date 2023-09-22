<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class RoutingGateway extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/routing/gateway";
        $this->model_name = "RoutingGateway";
        $this->tag = "ROUTING";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-system-gateways"];
        $this->post_privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->patch_privileges = ["page-all", "page-system-gateways-editgateway"];
        $this->delete_privileges = ["page-all", "page-system-gateways-editgateway"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
