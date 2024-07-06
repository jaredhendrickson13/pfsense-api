<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular NetworkInterface Model object at
 * /api/v2/interface.
 */
class NetworkInterfaceEndpoint extends Endpoint {
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = '/api/v2/interface';
        $this->model_name = 'NetworkInterface';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
