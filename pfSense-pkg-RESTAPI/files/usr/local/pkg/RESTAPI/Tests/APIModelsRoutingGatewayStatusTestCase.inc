<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RoutingGatewayStatus;

class APIModelsRoutingGatewayStatusTestCase extends TestCase
{
    /**
     * Checks that we are able to read gateway status using the RoutingGatewayStatus model.
     */
    public function test_read(): void {
        # Read a gateway status and ensure the field values are correctly populated
        $gateway_status = new RoutingGatewayStatus(id: 0);

        $this->assert_type($gateway_status->name->value, "string");
        $this->assert_is_not_empty($gateway_status->name->value);

        $this->assert_type($gateway_status->srcip->value, "string");
        $this->assert_is_not_empty($gateway_status->srcip->value);
        $this->assert_is_true(
            is_ipaddrv4($gateway_status->srcip->value) or is_ipaddrv6($gateway_status->srcip->value)
        );

        $this->assert_type($gateway_status->monitorip->value, "string");
        $this->assert_is_not_empty($gateway_status->monitorip->value);
        $this->assert_is_true(
            is_ipaddrv4($gateway_status->monitorip->value) or is_ipaddrv6($gateway_status->monitorip->value)
        );

        $this->assert_type($gateway_status->delay->value, "double");
        $this->assert_is_not_empty($gateway_status->delay->value);

        $this->assert_type($gateway_status->stddev->value, "double");
        $this->assert_is_not_empty($gateway_status->stddev->value);

        $this->assert_type($gateway_status->loss->value, "double");
        $this->assert_is_not_empty($gateway_status->loss->value);
        $this->assert_is_greater_than_or_equal($gateway_status->loss->value, 0);
        $this->assert_is_less_than_or_equal($gateway_status->loss->value, 100);

        $this->assert_type($gateway_status->status->value, "string");
        $this->assert_is_not_empty($gateway_status->status->value);

        $this->assert_type($gateway_status->status->value, "string");
        $this->assert_is_not_empty($gateway_status->substatus->value);
    }
}