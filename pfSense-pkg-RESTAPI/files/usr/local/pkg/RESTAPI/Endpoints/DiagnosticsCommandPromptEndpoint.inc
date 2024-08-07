<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with singular CommandPrompt Model objects at /api/v2/diagnostics/command_prompt.
 */
class DiagnosticsCommandPromptEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/diagnostics/command_prompt';
        $this->model_name = 'CommandPrompt';
        $this->request_method_options = ['POST'];
        $this->post_help_text = 'Executes a shell command.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
