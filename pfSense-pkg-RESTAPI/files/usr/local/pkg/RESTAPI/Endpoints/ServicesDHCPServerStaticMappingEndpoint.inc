<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular DHCPServerStaticMapping Model object at
 * /api/v2/services/dhcp_server/static_mapping.
 */
class ServicesDHCPServerStaticMappingEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server/static_mapping';
        $this->model_name = 'DHCPServerStaticMapping';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
