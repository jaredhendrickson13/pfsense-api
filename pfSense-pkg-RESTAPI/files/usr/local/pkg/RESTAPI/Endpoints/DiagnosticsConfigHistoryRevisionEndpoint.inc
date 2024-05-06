<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single ConfigHistoryRevision Model at
 * /api/v2/diagnostics/config_history/revision.
 */
class DiagnosticsConfigHistoryRevisionEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/config_history/revision';
        $this->model_name = 'ConfigHistory';
        $this->request_method_options = ['GET', 'DELETE'];
        $this->delete_help_text = "Deletes a configuration revision along with it's associated backup file.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
