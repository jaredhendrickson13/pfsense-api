<?php

namespace API\Endpointss;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallRules extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/rules";
        $this->model_name = "FirewallRule";
        $this->many = true;

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET"];
        $this->get_privileges = ["page-all", "page-firewall-rules"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
