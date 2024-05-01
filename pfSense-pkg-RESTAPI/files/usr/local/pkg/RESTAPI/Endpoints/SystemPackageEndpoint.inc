<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single Package Model object at 
 * /api/v2/system/package.
 */
class SystemPackageEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/package';
        $this->model_name = 'Package';
        $this->request_method_options = ['GET', 'POST', 'DELETE'];
        $this->get_help_text = 'Reads an installed Package.';
        $this->post_help_text = 'Installs a new Package.';
        $this->delete_help_text = 'Deletes an existing Package.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
