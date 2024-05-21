<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many AvailableInterface Model objects at
 * /api/v2/interface/available_interfaces
 */
class InterfaceAvailableInterfacesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/interface/available_interfaces';
        $this->model_name = 'AvailableInterface';
        $this->many = true;
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
