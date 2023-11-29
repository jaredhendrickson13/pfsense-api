<?php

namespace api\endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class SystemAPIVersion extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/system/api/version";
        $this->model_name = "APIVersion";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["GET"];
        $this->get_privileges = ["page-all", "page-system-api"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
