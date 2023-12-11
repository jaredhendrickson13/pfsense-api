<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class StatusService extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/status/service";
        $this->model_name = "Service";
        $this->request_method_options = ["POST"];
        $this->post_help_text = "Triggers a start, stop or restart action for an existing Service";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
