<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many OneToOneNATMapping Model objects at /api/v2/firewall/nat/one_to_one/mappings.
 */
class FirewallNATOneToOneMappingsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/nat/one_to_one/mappings';
        $this->model_name = 'OneToOneNATMapping';
        $this->many = true;
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
