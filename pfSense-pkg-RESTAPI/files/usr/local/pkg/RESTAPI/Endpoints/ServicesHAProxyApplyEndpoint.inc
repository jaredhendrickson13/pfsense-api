<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the HAProxyApply Model object at
 * /api/v2/services/haproxy/apply.
 */
class ServicesHAProxyApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/haproxy/apply';
        $this->model_name = 'HAProxyApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending HAProxy change status.';
        $this->post_help_text = 'Apply pending HAProxy changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
