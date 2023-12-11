<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class DiagnosticsCommandPrompt extends Endpoint
{
    public function __construct() {
        # Set Endpoint attributes
        $this->url = "/api/v2/diagnostics/command_prompt";
        $this->model_name = "CommandPrompt";
        $this->request_method_options = ["POST"];
        $this->post_help_text = "Executes a shell command.";

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
