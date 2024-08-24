<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNServerRouteStatus Model objects at
 * /api/v2/status/openvpn/server/route.
 */
class StatusOpenVPNServerRouteEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/openvpn/server/route';
        $this->model_name = 'OpenVPNServerRouteStatus';
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
