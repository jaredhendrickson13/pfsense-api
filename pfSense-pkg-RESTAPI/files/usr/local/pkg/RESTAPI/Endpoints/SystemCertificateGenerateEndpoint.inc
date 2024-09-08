<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificateGenerate object at
 * /api/v2/system/certificate/generate.
 */
class SystemCertificateGenerateEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate/generate';
        $this->model_name = 'CertificateGenerate';
        $this->request_method_options = ['POST'];

        # Set help text
        $this->post_help_text = 'Generate a new internal certificate.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
