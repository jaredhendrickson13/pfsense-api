<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the SystemHostname Model object at 
 * /api/v2/system/hostname.
 */
class SystemHostnameEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/hostname';
        $this->model_name = 'SystemHostname';
        $this->request_method_options = ['GET', 'PATCH'];
        $this->get_help_text = 'Reads the current system hostname.';
        $this->patch_help_text = 'Updates the system hostname.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
