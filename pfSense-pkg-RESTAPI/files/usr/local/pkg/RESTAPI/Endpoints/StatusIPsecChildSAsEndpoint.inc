<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many IPsecChildSAStatus Model objects at
 * /api/v2/status/ipsec/child_sas.
 */
class StatusIPsecChildSAsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/ipsec/child_sas';
        $this->model_name = 'IPsecChildSAStatus';
        $this->many = true;
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
