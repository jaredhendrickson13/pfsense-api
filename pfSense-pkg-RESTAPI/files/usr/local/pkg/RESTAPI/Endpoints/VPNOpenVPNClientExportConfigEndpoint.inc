<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNExportConfig Model object at
 * /api/v2/vpn/openvpn/clientexport.
 */
class VPNOpenVPNClientExportConfigEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/openvpn/client_export/config';
        $this->model_name = 'OpenVPNClientExportConfig';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];
        $this->many = false;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
