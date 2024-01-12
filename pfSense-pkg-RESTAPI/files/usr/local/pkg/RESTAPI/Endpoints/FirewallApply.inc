<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class FirewallApply extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/firewall/apply";
        $this->model_name = "FirewallApply";
        $this->request_method_options = ["GET", "POST"];
        $this->get_help_text = "Read pending firewall change status.";
        $this->post_help_text = "Apply pending firewall changes.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
