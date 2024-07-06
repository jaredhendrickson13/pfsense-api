<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the DNSResolverApply Model object at
 * /api/v2/services/dns_resolver/apply.
 */
class ServicesDNSResolverApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_resolver/apply';
        $this->model_name = 'DNSResolverApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending DNS Resolver change status.';
        $this->post_help_text = 'Apply pending DNS Resolver changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
