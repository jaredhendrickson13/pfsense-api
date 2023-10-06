<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallVirtualIPs extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/virtual_ips";
        $this->many = true;
        $this->model_name = "VirtualIP";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET"];
        $this->get_privileges = ["page-all", "page-firewall-virtualipaddresses"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
