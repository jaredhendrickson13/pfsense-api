<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many ConfigHistoryRevision Models at
 * /api/v2/diagnostics/config_history/revisions.
 */
class DiagnosticsConfigHistoryRevisionsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/config_history/revisions';
        $this->model_name = 'ConfigHistoryRevision';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
