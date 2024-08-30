<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many CertificateRevocationList Model objects at
 * /api/v2/system/crls.
 */
class SystemCRLsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/crls';
        $this->model_name = 'CertificateRevocationList';
        $this->request_method_options = ['GET', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
