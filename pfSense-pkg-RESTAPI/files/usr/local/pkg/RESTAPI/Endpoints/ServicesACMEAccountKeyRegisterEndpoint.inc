<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the ACMEAccountKeyRegister Model at /api/v2/services/acme/account_key/register
 */
class ServicesACMEAccountKeyRegisterEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/account_key/register';
        $this->model_name = 'ACMEAccountKeyRegister';
        $this->request_method_options = ['POST'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
