<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single ACMECertificateIssue Model object at
 * /api/v2/services/acme/certificate/issue.
 */
class ServicesACMECertificateIssueEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/acme/certificate/issue';
        $this->model_name = 'ACMECertificateIssue';
        $this->request_method_options = ['POST'];

        # Set help text
        $this->post_help_text =
            'Issue an ACME Certificate.<br><br>Note: This endpoint simply starts the issue ' .
            'process but does not attempt to determine whether the outcome was successful. You must manually parse ' .
            'the result to determine the outcome.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
