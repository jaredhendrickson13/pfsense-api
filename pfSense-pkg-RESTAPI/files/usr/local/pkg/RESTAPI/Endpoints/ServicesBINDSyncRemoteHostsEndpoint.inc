<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many BINDSyncRemoteHost Model objects at
 * /api/v2/services/bind/sync/remote_hosts.
 */
class ServicesBINDSyncRemoteHostsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/bind/sync/remote_hosts';
        $this->model_name = 'BINDSyncRemoteHost';
        $this->request_method_options = ['GET', 'PUT', 'DELETE'];
        $this->many = true;

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
