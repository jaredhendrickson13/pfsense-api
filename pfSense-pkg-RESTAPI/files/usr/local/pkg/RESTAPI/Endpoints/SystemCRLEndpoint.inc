<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class SystemCRLEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/crl';
        $this->model_name = 'CertificateRevocationList';
        $this->request_method_options = ['GET', 'POST', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
