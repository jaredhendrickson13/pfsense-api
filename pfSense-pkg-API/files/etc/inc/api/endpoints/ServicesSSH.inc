<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class ServicesSSH extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/services/ssh";
        $this->model_name = "SSH";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_privileges = ["page-all", "page-system-advanced-admin"];
        $this->patch_privileges = ["page-all", "page-system-advanced-admin"];

        # Assign documentation help text for each request method
        $this->get_help_text = "Reads the current SSH server settings.";
        $this->patch_help_text = "Updates the current SSH server settings.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
