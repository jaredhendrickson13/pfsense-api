<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificateSigningRequestSign object at
 * /api/v2/system/certificate/signing_request/sign.
 */
class SystemCertificateSigningRequestSignEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate/signing_request/sign';
        $this->model_name = 'CertificateSigningRequestSign';
        $this->request_method_options = ['POST'];

        # Set help text
        $this->post_help_text =
            'Signs an existing Certificate Signing Request (CSR) with an ' . 'existing Certificate Authority (CA).';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
