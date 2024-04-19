<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class ServicesHAProxyFiles extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/files';
        $this->model_name = 'HAProxyFile';
        $this->request_method_options = ['GET', 'PUT'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
