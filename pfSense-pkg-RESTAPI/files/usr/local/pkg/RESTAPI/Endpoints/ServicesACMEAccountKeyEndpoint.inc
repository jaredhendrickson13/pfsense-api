<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single ACMEAccountKey Model object at /api/v2/services/acme/account_key.
 */
class ServicesACMEAccountKeyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/account_key';
        $this->model_name = 'ACMEAccountKey';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();

        # Set help text. This must be done after constructing the parent model to append to default help text.
        $this->post_help_text .=
            '<br><br>Note: You must register the account key with the ACME server after creating ' .
            'it. You can register the account key using the /api/v2/services/acme/account_key/register endpoint.';
        $this->patch_help_text .=
            '<br><br>Note: You may need to re-register the account key with the ACME server ' .
            'after updating. You can register the account key using the /api/v2/services/acme/account_key/register ' .
            'endpoint.';
    }
}
