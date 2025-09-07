<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the SystemTimezone Model object at
 * /api/v2/system/timezone.
 */
class SystemTimezoneEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/timezone';
        $this->model_name = 'SystemTimezone';
        $this->request_method_options = ['GET', 'PATCH'];
        $this->get_help_text = 'Reads the current system timezone.';
        $this->patch_help_text = 'Updates the system timezone.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
