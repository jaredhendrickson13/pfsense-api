<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class InterfaceVLANs extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/interface/vlans";
        $this->model_name = "InterfaceVLAN";
        $this->many = true;

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PUT"];
        $this->get_privileges = ["page-all", "page-interfaces-vlan"];
        $this->put_privileges = ["page-all", "page-interfaces-vlan-edit"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
