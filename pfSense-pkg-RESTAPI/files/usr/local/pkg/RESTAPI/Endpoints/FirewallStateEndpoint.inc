<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular FirewallState Model object at /api/v2/firewall/state.
 */
class FirewallStateEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/state';
        $this->model_name = 'FirewallState';
        $this->request_method_options = ['GET', 'DELETE'];

        # Set documentation help text
        $this->get_help_text =
            'Reads an existing Firewall State. Please note that the firewall state table changes very quickly which may '.
            'result in the state\'s `id` suddenly changing. It is recommended to use the /api/v2/firewall/states '.
            'endpoint with a query to find the state you are looking for.';
        $this->delete_help_text =
            'Kills an existing Firewall State. Please note that the firewall state table changes very quickly which may '.
            'result in the state\'s `id` suddenly changing. Use caution when deleting states.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
