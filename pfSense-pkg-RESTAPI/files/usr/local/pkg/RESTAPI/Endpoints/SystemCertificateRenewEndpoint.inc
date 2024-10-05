<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificateRenew object at
 * /api/v2/system/certificate/renew.
 */
class SystemCertificateRenewEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate/renew';
        $this->model_name = 'CertificateRenew';
        $this->request_method_options = ['POST'];

        # Set help text
        $this->post_help_text = 'Renews an existing Certificate.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
