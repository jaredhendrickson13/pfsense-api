<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Fields\InterfaceField;
use API\Models\APISettings;

class APIFieldsInterfaceFieldTestCase extends TestCase
{
    # TODO: Needs tests to ensure CARP interfaces and interface groups become available choices when the
    # TODO: `allow_carp_interface` and `allow_interface_groups` properties are enabled.

    /**
     * Checks that the InterfaceField correctly determines all interface choices.
     */
    public function test_get_interface_choices() {
        # Variables
        $api_settings = new APISettings();

        # Check standard interfaces with `represent_interfaces_as` set to `internal`.
        $api_settings->represent_interfaces_as->value = "internal";
        $api_settings->update();
        $test_field = new InterfaceField();
        $this->assert_is_true(in_array("wan", $test_field->choices));
        $this->assert_is_true(in_array("lan", $test_field->choices));

        # Check standard interfaces with `represent_interfaces_as` set to `description`.
        $api_settings->represent_interfaces_as->value = "description";
        $api_settings->update();
        $test_field = new InterfaceField();
        $this->assert_is_true(in_array("WAN", $test_field->choices));
        $this->assert_is_true(in_array("LAN", $test_field->choices));

        # Check standard interfaces with `represent_interfaces_as` set to `physical`.
        $api_settings->represent_interfaces_as->value = "physical";
        $api_settings->update();
        $test_field = new InterfaceField();
        $this->assert_is_true(in_array("em0", $test_field->choices));
        $this->assert_is_true(in_array("em1", $test_field->choices));
    }

    /**
     * Checks that the InterfaceField's `allow_ipsec_interface` parameter allows the IPsec `enc0` interface to become
     * a choice.
     */
    public function test_get_interface_choices_with_allow_ipsec_interface() {
        # First ensure it is not a valid choice by default
        $test_field = new InterfaceField();
        $this->assert_is_false(in_array("enc0", $test_field->choices));

        # Ensure `enc0` becomes a valid choice when `allow_ipsec_interface` is enabled
        $test_field = new InterfaceField(allow_ipsec_interface: true);
        $this->assert_is_true(in_array("enc0", $test_field->choices));
    }

    /**
     * Checks that the InterfaceField's `allow_openvpn_interface` parameter allows the IPsec `openvpn` interface to become
     * a choice.
     */
    public function test_get_interface_choices_with_allow_openvpn_interface() {
        # First ensure it is not a valid choice by default
        $test_field = new InterfaceField();
        $this->assert_is_false(in_array("openvpn", $test_field->choices));

        # Ensure `openvpn` becomes a valid choice when `allow_ipsec_interface` is enabled
        $test_field = new InterfaceField(allow_openvpn_interface: true);
        $this->assert_is_true(in_array("openvpn", $test_field->choices));
    }

    /**
     * Checks that the InterfaceField's to_internal() method correctly translates representation values to their
     * internal values.
     */
    public function test_to_internal() {
        # Variables
        $api_settings = new APISettings();

        # Check with `represent_interfaces_as` set to `internal`.
        $api_settings->represent_interfaces_as->value = "internal";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->value = "wan";
        $this->assert_equals($test_field->to_internal(), "wan");

        # Check with `represent_interfaces_as` set to `description`.
        $api_settings->represent_interfaces_as->value = "description";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->value = "WAN";
        $this->assert_equals($test_field->to_internal(), "wan");

        # Check with `represent_interfaces_as` set to `physical`.
        $api_settings->represent_interfaces_as->value = "physical";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->value = "em0";
        $this->assert_equals($test_field->to_internal(), "wan");
    }

    /**
     * Checks that the InterfaceField's from_internal() method correctly translates internal values to their
     * representation values.
     */
    public function test_from_internal() {
        # Variables
        $api_settings = new APISettings();

        # Check with `represent_interfaces_as` set to `internal`.
        $api_settings->represent_interfaces_as->value = "internal";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->from_internal("wan");
        $this->assert_equals($test_field->value, "wan");

        # Check with `represent_interfaces_as` set to `description`.
        $api_settings->represent_interfaces_as->value = "description";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->from_internal("wan");
        $this->assert_equals($test_field->value, "WAN");

        # Check with `represent_interfaces_as` set to `physical`.
        $api_settings->represent_interfaces_as->value = "physical";
        $api_settings->update();
        $test_field = new InterfaceField();
        $test_field->from_internal("wan");
        $this->assert_equals($test_field->value, "em0");
    }
}
