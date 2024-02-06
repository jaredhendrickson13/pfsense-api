<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class ServicesDHCPServerBackendEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server/backend';
        $this->model_name = 'DHCPServerBackend';
        $this->request_method_options = ['PATCH'];
        $this->patch_help_text = 'Select the DHCP Server Backend.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
