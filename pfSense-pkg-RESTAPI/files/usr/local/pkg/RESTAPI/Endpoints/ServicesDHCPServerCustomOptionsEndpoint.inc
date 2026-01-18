<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple DHCPServerCustomOption Model objects at
 * /api/v2/services/dhcp_server/custom_options.
 */
class ServicesDHCPServerCustomOptionsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server/custom_options';
        $this->model_name = 'DHCPServerCustomOption';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
