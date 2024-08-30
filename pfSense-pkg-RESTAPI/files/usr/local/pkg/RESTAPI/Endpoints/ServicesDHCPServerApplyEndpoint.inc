<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with singular DHCPServerApply Model objects at /api/v2/services/dhcp_server/apply.
 */
class ServicesDHCPServerApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/dhcp_server/apply';
        $this->model_name = 'DHCPServerApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending DHCP Server change status.';
        $this->post_help_text = 'Apply pending DHCP Server changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
