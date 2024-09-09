<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular CertificatePKCS12Export object at
 * /api/v2/system/certificate/pkcs12/export.
 */
class SystemCertificatePKCS12ExportEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/certificate/pkcs12/export';
        $this->model_name = 'CertificatePKCS12Export';
        $this->request_method_options = ['POST'];
        $this->encode_content_handlers = ['BinaryContentHandler']; # Only allow binary DLs (application/octet-stream)
        # Set help text
        $this->post_help_text = 'Exports an existing certificate as a PKCS#12 archive.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
