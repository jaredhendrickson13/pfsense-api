<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many HAProxyEmailMailer Model objects at
 * /api/v2/services/haproxy/settings/email_mailers.
 */
class ServicesHAProxySettingsEmailMailersEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/settings/email_mailers';
        $this->model_name = 'HAProxyEmailMailer';
        $this->many = true;
        $this->request_method_options = ['GET', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
