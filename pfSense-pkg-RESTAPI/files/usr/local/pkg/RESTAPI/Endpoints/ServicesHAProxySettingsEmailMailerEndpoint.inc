<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular HAProxyEmailMailer Model object at 
 * /api/v2/services/haproxy/settings/email_mailer.
 */
class ServicesHAProxySettingsEmailMailerEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/settings/email_mailer';
        $this->model_name = 'HAProxyEmailMailer';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
