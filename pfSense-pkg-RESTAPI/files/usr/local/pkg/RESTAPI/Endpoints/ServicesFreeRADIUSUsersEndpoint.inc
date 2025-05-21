<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNExport Model object at
 * /api/v2/vpn/openvpn/clientexport.
 */
class ServicesFreeRADIUSUsersEndpoint extends Endpoint {
    public function __construct() {
        /**
         *  Set Endpoint attributes
         */
        $this->url = '/api/v2/services/freeradius/users';
        $this->model_name = 'FreeRADIUSUser';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
