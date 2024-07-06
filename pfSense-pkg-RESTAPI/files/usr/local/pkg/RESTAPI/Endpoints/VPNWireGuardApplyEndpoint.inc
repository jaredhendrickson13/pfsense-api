<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the WireGuardApply Model object at
 * /api/v2/vpn/wireguard/apply.
 */
class VPNWireGuardApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/wireguard/apply';
        $this->model_name = 'WireGuardApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending WireGuard change status.';
        $this->post_help_text = 'Apply pending WireGuard changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
