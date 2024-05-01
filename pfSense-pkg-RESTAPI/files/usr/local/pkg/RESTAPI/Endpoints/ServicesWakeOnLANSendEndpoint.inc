<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the WakeOnLANSend Model object at 
 * /api/v2/services/wake_on_lan/send.
 */
class ServicesWakeOnLANSendEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/services/wake_on_lan/send';
        $this->model_name = 'WakeOnLANSend';
        $this->request_method_options = ['POST'];
        $this->post_help_text = 'Sends a Wake-on-LAN magic packet.';

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
