<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many OpenVPNClientExportConfig Model objects at
 * /api/v2/vpn/openvpn/client_export/configs.
 */
class VPNOpenVPNClientExportConfigsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/openvpn/client_export/configs';
        $this->model_name = 'OpenVPNClientExportConfig';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
