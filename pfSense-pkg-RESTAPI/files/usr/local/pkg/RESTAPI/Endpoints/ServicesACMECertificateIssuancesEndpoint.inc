<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many ACMECertificateIssue Model objects at
 * /api/v2/services/acme/certificate/issuances.
 */
class ServicesACMECertificateIssuancesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/certificate/issuances';
        $this->model_name = 'ACMECertificateIssue';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Set help text
        $this->get_help_text =
            'Reads all existing ACME certificate issuances. This endpoint is primarily intended to ' .
            'check the result of the ACME certificate issuance process initiated by the POST ' .
            '/api/v2/services/acme/certificate/issue endpoint. This endpoint is unable to determine the status of ' .
            'issuances that were initiated outside of the REST API.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
