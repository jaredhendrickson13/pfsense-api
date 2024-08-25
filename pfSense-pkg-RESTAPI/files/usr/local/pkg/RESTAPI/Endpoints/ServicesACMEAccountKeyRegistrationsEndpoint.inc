<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many ACMEAccountKeyRegister Models at /api/v2/services/acme/account_key/registrations
 */
class ServicesACMEAccountKeyRegistrationsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/account_key/registrations';
        $this->model_name = 'ACMEAccountKeyRegister';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Set help text
        $this->get_help_text =
            'Reads all existing ACME account registrations. This endpoint is primarily intended to ' .
            'check the result of the ACME account key registration process initiated by the POST ' .
            '/api/v2/services/acme/account_key/register endpoint. This endpoint is unable to determine the status of ' .
            'registrations that were initiated outside of the REST API.';
        # Construct the parent Endpoint object
        parent::__construct();
    }
}
