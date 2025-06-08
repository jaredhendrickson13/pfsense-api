<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a many interface Model object at
 * /api/v2/services/freeradius/interfaces
 */
class ServicesFreeRADIUSInterfacesEndpoint extends Endpoint {
    public function __construct() {
        /**
         *  Set Endpoint attributes
         */
        $this->url = '/api/v2/services/freeradius/interfaces';
        $this->model_name = 'FreeRADIUSInterface';
        $this->many = true;
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
