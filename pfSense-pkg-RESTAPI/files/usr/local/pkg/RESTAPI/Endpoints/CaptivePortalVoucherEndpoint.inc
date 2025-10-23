<?php

namespace RESTAPI\Endpoints;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Endpoint;

/**
 * Defines an Endpoint for interacting with many CaptivePortalVoucher Model objects at /api/v2/captive-portal/voucher.
 */
class CaptivePortalVoucherEndpoint extends Endpoint {
    public function __construct() {
        # Set Endpoint attributes
        $this->url = '/api/v2/captive-portal/voucher';
        $this->model_name = 'CaptivePortalVoucher';
        $this->request_method_options = ['GET', 'POST'];

        # Construct the parent Endpoint object
        parent::__construct();
    }
}
