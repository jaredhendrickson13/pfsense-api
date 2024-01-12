<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemDNS extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/dns";
        $this->model_name = "SystemDNS";
        $this->request_method_options = ["GET", "PATCH"];
        $this->get_help_text = "Reads the current system DNS settings.";
        $this->patch_help_text = "Updates the current system DNS settings.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
