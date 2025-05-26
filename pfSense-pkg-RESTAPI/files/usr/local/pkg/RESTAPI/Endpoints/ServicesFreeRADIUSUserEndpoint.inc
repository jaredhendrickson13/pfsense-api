<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single FreeRADIUSUser Model object at
 * /api/v2/services/freeradius/user
 */
class ServicesFreeRADIUSUserEndpoint extends Endpoint {
    public function __construct() {
        /**
         *  Set Endpoint attributes
         */
        $this->url = '/api/v2/services/freeradius/user';
        $this->model_name = 'FreeRADIUSUser';
        $this->many = false;
        $this->request_method_options = ['GET', 'POST', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}

