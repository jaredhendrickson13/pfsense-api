<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with the EmailNotificationSettings Model at
 * /api/v2/system/notifications/email_settings.
 */
class SystemNotificationsEmailSettingsEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/system/notifications/email_settings';
        $this->model_name = 'EmailNotificationSettings';
        $this->request_method_options = ['GET', 'PATCH'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
