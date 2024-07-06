<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with a single RESTAPIAccessListEntry object at
 * /api/v2/system/restapi/access_list/entry.
 */
class SystemRESTAPIAccessListEntryEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/restapi/access_list/entry';
        $this->model_name = 'RESTAPIAccessListEntry';
        $this->request_method_options = ['GET', 'POST', 'PATCH', 'DELETE'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
