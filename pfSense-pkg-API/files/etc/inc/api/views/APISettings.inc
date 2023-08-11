<?php

namespace API\Views;

require_once("api/auto_loader.inc");

use API\Core\View;

class APISettings extends View
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/api/settings";
        $this->model_name = "APISettings";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_privileges = ["page-all", "page-system-api"];
        $this->patch_privileges = ["page-all", "page-system-api"];

        # Assign documentation help text for each request method
        $this->get_help_text = "Reads the current API settings.";
        $this->patch_help_text = "Updates the current API settings.";

        # Construct the parent View object
        parent::__construct();
    }
}
