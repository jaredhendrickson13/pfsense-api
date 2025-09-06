<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with singular Table Model objects at /api/v2/diagnostics/table.
 */
class DiagnosticsTableEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/table';
        $this->model_name = 'Table';
        $this->request_method_options = ['GET', 'DELETE'];

        # Set help texts
        $this->get_help_text = 'Retrieves the entries in a specified table.';
        $this->delete_help_text =
            'Flushes all entries in a specified table. Please note this does not ' .
            'delete the table itself, only its entries.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
