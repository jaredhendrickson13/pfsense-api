<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesDHCPServer extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/services/dhcp_server";
        $this->model_name = "DHCPServer";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_privileges = ["page-all", "page-services-dhcpserver"];
        $this->patch_privileges = ["page-all", "page-services-dhcpserver"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
