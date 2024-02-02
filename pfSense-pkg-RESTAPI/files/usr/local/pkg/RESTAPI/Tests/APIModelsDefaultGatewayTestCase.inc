<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DefaultGateway;

class APIModelsDefaultGatewayTestCase extends TestCase
{
    /**
     * Ensure we can update the DefaultGateway for both IPv4 and IPv6
     */
    public function test_update(): void
    {
        $this->assert_does_not_throw(
            callable: function () {
                $default_gw = new DefaultGateway(defaultgw4: '', defaultgw6: '-');
                $default_gw->update(apply: true);
            },
        );
    }
}
