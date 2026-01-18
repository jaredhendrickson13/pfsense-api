<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple DHCPServerAddressPool Model objects at
 * /api/v2/services/dhcp_server/address_pools.
 */
class ServicesDHCPServerAddressPoolsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server/address_pools';
        $this->model_name = 'DHCPServerAddressPool';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
