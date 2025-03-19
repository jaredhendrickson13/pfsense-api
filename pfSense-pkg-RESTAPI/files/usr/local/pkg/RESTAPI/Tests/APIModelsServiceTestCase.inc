<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Service;

class APIModelsServiceTestCase extends TestCase {
    /**
     * Checks that the `get_services()` method correctly identifies all available services.
     */
    public function test_get_services(): void {
        # Ensure expected services are found in the method's response
        $expected_services = ['unbound', 'ntpd', 'syslogd', 'dhcpd', 'dpinger', 'sshd'];

        # Ensure each expected service is found in the method's response
        foreach ($expected_services as $service) {
            $this->assert_is_true(
                in_array($service, array_column(Service::get_services(), 'name')),
                "Expected $service to be in the list of services",
            );
        }
    }

    /**
     * Checks that a Service can be stopped, started and restarted.
     */
    public function test_can_perform_service_actions(): void {
        # Obtain a Service to test with
        $test_service = Service::query(name: "ntpd")->first();

        # Ensure the service can be stopped
        $test_service->action->value = 'stop';
        $test_service->create();
        $this->assert_is_false($test_service->status->value);

        # Ensure the service can be started
        $test_service->action->value = 'start';
        $test_service->create();
        $this->assert_is_true($test_service->status->value);

        # Ensure the service can be started
        $test_service->action->value = 'restart';
        $test_service->create();
        $this->assert_is_true($test_service->status->value);
    }
}
