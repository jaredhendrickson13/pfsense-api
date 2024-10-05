<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single IPseChildcSAStatus Model object at
 * /api/v2/status/ipsec/child_sa.
 */
class StatusIPsecChildSAEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/ipsec/child_sa';
        $this->model_name = 'IPsecChildSAStatus';
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
