<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\Service;

class APIModelsServiceTestCase extends TestCase
{
    /**
     * Checks that the `get_services()` method correctly identifies all available services.
     */
    public function test_get_services() {
        # Ensure expected services are found in the method's response
        $expected_services = ["unbound", "ntpd", "syslogd", "dhcpd", "dpinger", "sshd"];

        # Loop through each identified service and ensure it is expected
        foreach (Service::get_services() as $service) {
            $this->assert_is_true(in_array($service["name"], $expected_services));
        }
    }

    /**
     * Checks that the `get_service_name_choices()` method correctly identifies all available service names.
     */
    public function test_get_service_name_choices() {
        # Ensure expected services are found in the method's response
        $expected_services = ["unbound", "ntpd", "syslogd", "dhcpd", "dpinger", "sshd"];

        # Loop through each identified service and ensure it is expected
        foreach ((new Service())->get_service_name_choices() as $service) {
            $this->assert_is_true(in_array($service, $expected_services));
        }
    }

    /**
     * Checks that the `get_id_by_name()` correctly obtains the ID of the Service object with a specific `name`
     */
    public function test_get_id_by_name() {
        # Create a Service object to test with
        $test_service = new Service();

        # Obtain the ID of the `sshd` service and ensure the correct ID was given
        $sshd_id = $test_service->get_id_by_name("sshd");
        $this->assert_equals($sshd_id, $test_service->query(["name" => "sshd"])->first()->id);

    }

    /**
     * Checks that a Service can be stopped, started and restarted.
     */
    public function test_can_perform_service_actions() {
        # Define a Service to test with
        $test_service = new Service(data: ["name" => "ntpd"]);

        # Ensure the service can be stopped
        $test_service->action->value = "stop";
        $test_service->create();
        $this->assert_is_false($test_service->status->value);

        # Ensure the service can be started
        $test_service->action->value = "start";
        $test_service->create();
        $this->assert_is_true($test_service->status->value);

        # Ensure the service can be started
        $test_service->action->value = "restart";
        $test_service->create();
        $this->assert_is_true($test_service->status->value);
    }
}
