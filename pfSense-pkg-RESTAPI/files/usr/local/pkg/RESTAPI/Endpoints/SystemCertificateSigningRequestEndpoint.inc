<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificateSigningRequest object at
 * /api/v2/system/certificate/signing_request.
 */
class SystemCertificateSigningRequestEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate/signing_request';
        $this->model_name = 'CertificateSigningRequest';
        $this->request_method_options = ['POST'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
