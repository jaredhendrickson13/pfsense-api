<?php

namespace API\Endpoints;

require_once("api/auto_loader.inc");

use API\Core\Endpoint;

class DiagnosticsCommandPrompt extends Endpoint
{
    public function __construct() {
        # Assign the URL for this endpoint along with the Model it corresponds to.
        $this->url = "/api/v2/diagnostics/command_prompt";
        $this->model_name = "CommandPrompt";

        # Assign allowed request methods along with the privileges required for each
        $this->request_method_options = ["POST"];
        $this->post_help_text = "Executes a shell command.";
        $this->post_privileges = ["page-all", "page-diagnostics-command"];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
