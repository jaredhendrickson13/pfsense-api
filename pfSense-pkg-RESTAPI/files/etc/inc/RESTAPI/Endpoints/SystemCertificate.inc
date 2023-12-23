<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemCertificate extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/certificate";
        $this->model_name = "Certificate";
        $this->request_method_options = ["GET", "POST", "PATCH", "DELETE"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
