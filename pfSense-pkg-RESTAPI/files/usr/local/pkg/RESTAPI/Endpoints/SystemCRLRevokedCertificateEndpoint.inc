<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single CertificateRevocationListRevokedCertificate Model object at
 * /api/v2/system/crl/revoked_certificate.
 */
class SystemCRLRevokedCertificateEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/crl/revoked_certificate';
        $this->model_name = 'CertificateRevocationListRevokedCertificate';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
