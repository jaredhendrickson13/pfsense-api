<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificateAuthorityRenew object at
 * /api/v2/system/certificate_authority/renew.
 */
class SystemCertificateAuthorityRenewEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate_authority/renew';
        $this->model_name = 'CertificateAuthorityRenew';
        $this->request_method_options = ['POST'];

        # Set help text
        $this->post_help_text = 'Renews an existing Certificate Authority.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
