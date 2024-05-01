<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the InterfaceApply Model object at 
 * /api/v2/interface/apply.
 */
class InterfaceApplyEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/interface/apply';
        $this->model_name = 'InterfaceApply';
        $this->request_method_options = ['GET', 'POST'];
        $this->get_help_text = 'Read pending interfaces status.';
        $this->post_help_text = 'Apply pending interface changes.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
