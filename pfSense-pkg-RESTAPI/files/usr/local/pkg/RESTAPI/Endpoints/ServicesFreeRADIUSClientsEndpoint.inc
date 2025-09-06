<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a many Client Model object at
 * /api/v2/services/freeradius/clients
 */
class ServicesFreeRADIUSClientsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/freeradius/clients';
        $this->model_name = 'FreeRADIUSClient';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
