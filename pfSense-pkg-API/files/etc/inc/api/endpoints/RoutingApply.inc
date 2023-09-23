<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class RoutingApply extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/routing/apply";
        $this->model_name = "RoutingApply";
        $this->tag = "ROUTING";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST"];
        $this->get_privileges = [
            "page-all",
            "page-system-gateways",
            "page-system-gateways-editgateway",
            "page-system-staticroutes",
            "page-system-staticroutes-editroute"
        ];
        $this->post_privileges = [
            "page-all",
            "page-system-gateways",
            "page-system-gateways-editgateway",
            "page-system-staticroutes",
            "page-system-staticroutes-editroute"
        ];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
