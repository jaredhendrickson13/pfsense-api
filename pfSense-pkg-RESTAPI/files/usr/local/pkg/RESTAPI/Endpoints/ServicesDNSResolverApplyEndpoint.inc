<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class ServicesDNSResolverApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/apply';
        $this->model_name = 'DNSResolverApply';
        $this->request_method_options = ['GET', 'POST'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
