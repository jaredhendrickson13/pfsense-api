<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class InterfaceGroups extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/interface/groups";
        $this->model_name = "InterfaceGroup";
        $this->many = true;

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET"];
        $this->get_privileges = ["page-all", "page-interface-groups"];
        $this->put_privileges = ["page-all", "page-interface-groups-edit"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
