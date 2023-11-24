<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemHostname extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/hostname";
        $this->model_name = "SystemHostname";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_privileges = ["page-all", "page-system-generalsetup"];
        $this->patch_privileges = ["page-all", "page-system-generalsetup"];

        # Assign documentation help text for each request method
        $this->get_help_text = "Reads the current system hostname.";
        $this->patch_help_text = "Updates the system hostname.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
