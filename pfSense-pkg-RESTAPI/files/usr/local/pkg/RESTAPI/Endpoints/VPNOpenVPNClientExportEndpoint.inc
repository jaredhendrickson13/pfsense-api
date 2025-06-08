<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNExport Model object at
 * /api/v2/vpn/openvpn/clientexport.
 */
class VPNOpenVPNClientExportEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/openvpn/clientexport';
        $this->model_name = 'OpenVPNClientExport';
        $this->request_method_options = ['POST'];
        $this->many = false;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
