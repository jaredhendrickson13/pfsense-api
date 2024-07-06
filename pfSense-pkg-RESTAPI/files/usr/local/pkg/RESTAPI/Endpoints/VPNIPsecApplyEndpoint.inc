<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with singular IPsecApply Model objects at /api/v2/vpn/ipsec/apply.
 */
class VPNIPsecApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/ipsec/apply';
        $this->model_name = 'IPsecApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending IPsec change status.';
        $this->post_help_text = 'Apply pending IPsec changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
