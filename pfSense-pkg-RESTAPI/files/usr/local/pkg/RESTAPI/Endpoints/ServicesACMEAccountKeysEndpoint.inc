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

        # Set help text. This must be done after constructing the parent model to append to default help text.
        $this->put_help_text .=
            '<br><br>Note: You may need to register the account keys with the ACME server after ' .
            'replacing. You can register account keys using the /api/v2/services/acme/account_key/register endpoint.';
    }
}
