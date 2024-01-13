<?php

namespace RESTAPI\Endpoints;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Endpoint;

class SystemRESTAPISettingsSync extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/system/restapi/settings/sync";
        $this->model_name = "RESTAPISettingsSync";
        $this->request_method_options = ["PATCH"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
