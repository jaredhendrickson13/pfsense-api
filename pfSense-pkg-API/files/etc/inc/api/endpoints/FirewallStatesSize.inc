<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallStatesSize extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/states/size";
        $this->model_name = "FirewallStatesSize";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_privileges = ["page-all", "page-system-advanced-firewall"];
        $this->patch_privileges = ["page-all", "page-system-advanced-firewall"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
