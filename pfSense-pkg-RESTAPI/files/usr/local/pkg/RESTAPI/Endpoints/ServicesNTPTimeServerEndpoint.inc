<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class ServicesNTPTimeServerEndpoint extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/services/ntp/time_server";
        $this->model_name = "NTPTimeServer";
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];
        
        # Construct the parent Endpoint object
        parent::__construct();
    }
}
