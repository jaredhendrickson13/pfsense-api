<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many FirewallState Model objects at /api/v2/firewall/states.
 */
class FirewallStatesEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/firewall/states';
        $this->model_name = 'FirewallState';
        $this->request_method_options = ['GET', 'DELETE'];
        $this->many = true;
        $this->limit = 100; # Set default limit to 100

        # Set documentation help text
        $this->get_help_text .=
            'Reads all current Firewall States. A normal firewall may have thousands, millions, ' .
            'or even billions of firewall states registered at a time. This can quickly become too much for ' .
            'this endpoint to handle. It is strongly recommended to limit the number of objects obtained by the ' .
            'API using the `limit` parameter or using a query to only target specific states you are looking for.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
