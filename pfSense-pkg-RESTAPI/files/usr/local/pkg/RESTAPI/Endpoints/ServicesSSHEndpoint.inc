<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class ServicesSSHEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/ssh';
        $this->model_name = 'SSH';
        $this->request_method_options = ['GET', 'PATCH'];
        $this->get_help_text = 'Reads the current SSH server settings.';
        $this->patch_help_text = 'Updates the current SSH server settings.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
