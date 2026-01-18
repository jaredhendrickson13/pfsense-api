<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many RESTAPILog Models at /api/v2/status/logs/packages/restapi.
 */
class StatusLogsPackagesRESTAPIEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/status/logs/packages/restapi';
        $this->model_name = 'RESTAPILog';
        $this->many = true;
        $this->request_method_options = ['GET'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
