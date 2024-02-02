<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

class SystemRESTAPISettingsSyncEndpoint extends Endpoint
{
    public function __construct()
    {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/restapi/settings/sync';
        $this->model_name = 'RESTAPISettingsSync';
        $this->request_method_options = ['POST'];

        # Ensure the primary HA node can always sync to HA peers, regardless of its current settings
        $this->ignore_enabled = true;
        $this->ignore_interfaces = true;
        $this->ignore_read_only = true;
        $this->auth_methods = ['BasicAuth'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
