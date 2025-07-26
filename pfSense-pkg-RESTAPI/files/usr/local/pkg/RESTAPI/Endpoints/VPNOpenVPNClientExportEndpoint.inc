<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single OpenVPNExportConfig Model object at
 * /api/v2/vpn/openvpn/clientexport.
 */
class VPNOpenVPNClientExportEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/vpn/openvpn/client_export';
        $this->model_name = 'OpenVPNClientExport';
        $this->request_method_options = ['POST'];
        $this->encode_content_handlers = ['BinaryContentHandler', 'JSONContentHandler'];
        $this->many = false;

        # Set help texts
        $this->post_help_text =
            "Export an OpenVPN Client configuration.\n\n" .
            'Before using this endpoint, you must define a default export configuration for your ' .
            'OpenVPN server(s) using the the endpoint at /api/v2/openvpn/vpn/client_export/config ' .
            'as you will need its ID to use this endpoint. Any specific configurations made to this ' .
            'endpoint will override the default configurations, but will not store them in the pfSense ' .
            "configuration.\n\n" .
            "Exports of exe, zip and other binary file types MUST use the 'application/octet-stream' accept " .
            'type as their data is not serializable.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
