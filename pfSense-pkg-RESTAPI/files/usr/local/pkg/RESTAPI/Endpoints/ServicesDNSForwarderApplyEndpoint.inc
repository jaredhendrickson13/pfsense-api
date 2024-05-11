<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the DNSForwarderApply Model object at
 * /api/v2/services/dns_forwarder/apply.
 */
class ServicesDNSForwarderApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dns_forwarder/apply';
        $this->model_name = 'DNSForwarderApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending DNS Forwarder change status.';
        $this->post_help_text = 'Apply pending DNS Forwarder changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
