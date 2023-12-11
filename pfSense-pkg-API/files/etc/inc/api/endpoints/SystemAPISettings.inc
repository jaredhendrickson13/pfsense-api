<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemAPISettings extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/api/settings";
        $this->model_name = "APISettings";
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_help_text = "Reads the current API settings.";
        $this->patch_help_text = "Updates the current API settings.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
