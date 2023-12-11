<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class NetworkInterfaceApply extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/interface/apply";
        $this->model_name = "NetworkInterfaceApply";
        $this->request_method_options = ["GET", "POST"];
        $this->get_help_text = "Read pending interfaces status.";
        $this->post_help_text = "Apply pending interface changes.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
