<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the VirtualIPApply Model object at /api/v2/firewall/virtual_ip/apply.
 */
class FirewallVirtualIPApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/virtual_ip/apply';
        $this->model_name = 'VirtualIPApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending virtual IP status.';
        $this->post_help_text = 'Apply pending virtual IP changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
