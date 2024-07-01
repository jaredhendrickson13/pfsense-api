<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a singular FirewallAdvancedSettings Model object at
 * /api/v2/firewall/advanced_settings.
 */
class FirewallAdvancedSettingsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/advanced_settings';
        $this->model_name = 'FirewallAdvancedSettings';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
