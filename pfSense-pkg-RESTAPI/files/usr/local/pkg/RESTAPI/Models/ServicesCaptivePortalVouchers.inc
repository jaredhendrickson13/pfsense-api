<?php

namespace RESTAPI\Models;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Model;

/**
 * Defines a Model that interacts with captive portal vouchers.
 */
class ServicesCaptivePortalVouchers extends Model {
    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # TODO: Define Model attributes here
        $this->internal_callable = 'get_vouchers';

        # TODO: Define Model fields here

        # Needs to be constructed after Fields are defined
        parent::__construct($id, $parent_id, $data, ...$options);
    }

    protected function get_vouchers(): array {
        return [];
    }
}
