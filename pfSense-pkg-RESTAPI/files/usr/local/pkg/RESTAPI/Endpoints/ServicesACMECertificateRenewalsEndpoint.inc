<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many ACMECertificateRenew Model objects at
 * /api/v2/services/acme/certificate/renewals.
 */
class ServicesACMECertificateRenewalsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/certificate/renewals';
        $this->model_name = 'ACMECertificateRenew';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Set help text
        $this->get_help_text =
            'Reads all existing ACME certificate renewals. This endpoint is primarily intended to ' .
            'check the result of the ACME certificate renewal process initiated by the POST ' .
            '/api/v2/services/acme/certificate/renew endpoint. This endpoint is unable to determine the status of ' .
            'renewals that were initiated outside of the REST API.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
