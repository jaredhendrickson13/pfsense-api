<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a many ACMEAccountKey Model objects at /api/v2/services/acme/account_keys.
 */
class ServicesACMEAccountKeysEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/account_keys';
        $this->model_name = 'ACMEAccountKey';
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
