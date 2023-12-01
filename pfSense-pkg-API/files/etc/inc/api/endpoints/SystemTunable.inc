<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemTunable extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/tunable";
        $this->model_name = "SystemTunable";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        $this->get_privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->post_privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->patch_privileges = ["page-all", "page-system-advanced-sysctl"];
        $this->delete_privileges = ["page-all", "page-system-advanced-sysctl"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
