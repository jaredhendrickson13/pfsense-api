<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class InterfaceBridge extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/interface/bridge";
        $this->model_name = "InterfaceBridge";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all"];
        $this->post_privileges = ["page-all"];
        $this->patch_privileges = ["page-all"];
        $this->delete_privileges = ["page-all"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
