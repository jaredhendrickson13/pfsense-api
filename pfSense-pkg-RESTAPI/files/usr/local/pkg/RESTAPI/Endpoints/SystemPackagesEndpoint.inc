<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many Package Model objects at
 * /api/v2/system/packages.
 */
class SystemPackagesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/packages';
        $this->model_name = 'Package';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];
        $this->get_help_text = 'Reads all installed Packages.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
