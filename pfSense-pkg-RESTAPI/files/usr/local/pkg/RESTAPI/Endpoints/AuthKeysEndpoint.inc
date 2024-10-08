<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many RESTAPIKey Model objects at /api/v2/auth/key.
 */
class AuthKeysEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/auth/keys';
        $this->model_name = 'RESTAPIKey';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
