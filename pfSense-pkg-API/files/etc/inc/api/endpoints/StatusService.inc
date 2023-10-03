<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class StatusService extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/status/service";
        $this->model_name = "Service";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["POST"];
        $this->post_help_text = "Triggers a start, stop or restart action for an existing Service";
        $this->post_privileges = ["page-all", "page-status-services"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
