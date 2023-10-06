<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class FirewallVirtualIP extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/firewall/virtual_ip";
        $this->model_name = "VirtualIP";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-firewall-virtualipaddresses"];
        $this->post_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->patch_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];
        $this->delete_privileges = ["page-all", "page-firewall-virtualipaddress-edit"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
