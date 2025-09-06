<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with multiple Table Model objects at /api/v2/diagnostics/tables.
 */
class DiagnosticsTablesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/tables';
        $this->model_name = 'Table';
        $this->request_method_options = ['GET'];
        $this->many = true;

        # Set help texts
        $this->get_help_text =
            'Retrieves the entries from all tables. For systems with a large number of tables or ' .
            'entries, it is recommended to use pagination and/or querying to limit the amount of data returned.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
