<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\DHCPServerBackend;
use RESTAPI\Models\DHCPServerCustomOption;

class APIModelsDHCPServerCustomOptionTestCase extends TestCase
{
    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `text`.
     */
    public function test_validate_value_with_bad_text_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_NOT_CONTAIN_QUOTES_WITH_TEXT_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "text",
                    value: '"This is a bad value."'
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "text",
                    value: "This is a good value."
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `string`.
     */
    public function test_validate_value_with_bad_string_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_STRING_OR_HEX_STRING_WITH_STRING_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "string",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "string",
                    value: "\"This is a good value.\""
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "string",
                    value: "00:11:22:33:44:55"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `boolean`.
     */
    public function test_validate_value_with_bad_boolean_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_ONE_OF_BOOLEAN_OPTIONS_WITH_BOOLEAN_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "boolean",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "boolean",
                    value: "true"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "boolean",
                    value: "false"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "boolean",
                    value: "on"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "boolean",
                    value: "off"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `unsigned integer 8`.
     */
    public function test_validate_value_with_bad_unsigned_integer_8_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_8_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 8",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_8_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 8",
                    value: "256"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 8",
                    value: "255"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `unsigned integer 16`.
     */
    public function test_validate_value_with_bad_unsigned_integer_16_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_16_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 16",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_16_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 16",
                    value: "65536"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 16",
                    value: "65535"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `unsigned integer 32`.
     */
    public function test_validate_value_with_bad_unsigned_integer_32_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_32_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 32",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_UNSIGNED_INTEGER_32_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 32",
                    value: "4294967296"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "unsigned integer 32",
                    value: "4294967295"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `signed integer 8`.
     */
    public function test_validate_value_with_bad_signed_integer_8_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_SIGNED_INTEGER_8_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 8",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_SIGNED_INTEGER_8_TYPE",
            code: 400,
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 8",
                    value: "-129"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function() {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 8",
                    value: "-128"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `signed integer 16`.
     */
    public function test_validate_value_with_bad_signed_integer_16_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_SIGNED_INTEGER_16_TYPE",
            code: 400,
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 16",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `signed integer 32`.
     */
    public function test_validate_value_with_bad_signed_integer_32_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_SIGNED_INTEGER_32_TYPE",
            code: 400,
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 32",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_IN_RANGE_WITH_SIGNED_INTEGER_32_TYPE",
            code: 400,
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 32",
                    value: "-2147483649"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 32",
                    value: "-2147483648"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "signed integer 32",
                    value: "2147483647"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that the `value` field is validated correctly when the `type` field is set to `ip-address`.
     */
    public function test_validate_value_with_bad_ip_address_type_value(): void
    {
        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_VALID_IPV4_WITH_IP_ADDRESS_TYPE",
            code: 400,
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "ip-address",
                    value: "This is a bad value."
                );
                $option->validate();
            }
        );

        $this->assert_throws_response(
            response_id: "DHCP_SERVER_CUSTOM_OPTION_VALUE_MUST_BE_VALID_IPV4_WITH_IP_ADDRESS_TYPE",
            code: 400,
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "ip-address",
                    value: "192.168.1.256"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "ip-address",
                    value: "192.168.1.1"
                );
                $option->validate();
            }
        );

        $this->assert_does_not_throw(
            callable: function () {
                $option = new DHCPServerCustomOption(
                    parent_id: "lan",
                    number: 5,
                    type: "ip-address",
                    value: "example.com"
                );
                $option->validate();
            }
        );
    }

    /**
     * Checks that we can create, update and delete DHCP Server Custom Options.
     */
    public function test_crud():void {
        # Custom options are currently only available for ISC DHCP, ensure the backend is set
        $backend = new DHCPServerBackend(backend: "isc", async: false);
        $backend->update(apply: true);

        $option = new DHCPServerCustomOption(
            parent_id: "lan",
            number: 5,
            type: "text",
            value: "This is a good value.",
            async: false
        );
        $option->create(apply: true);
        $dhcpd_config = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_contains(
            $dhcpd_config,
            "option custom-$option->parent_id-$option->id code 5 = text"
        );
        $this->assert_str_contains(
            $dhcpd_config,
            "option custom-$option->parent_id-$option->id \"This is a good value.\""
        );

        $option->from_representation(
            parent_id: "lan",
            number: 10,
            type: "ip-address",
            value: "1.2.3.4"
        );
        $option->create(apply: true);
        $dhcpd_config = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_contains(
            $dhcpd_config,
            "option custom-$option->parent_id-$option->id code 10 = ip-address"
        );
        $this->assert_str_contains(
            $dhcpd_config,
            "option custom-$option->parent_id-$option->id 1.2.3.4"
        );

        $option->delete(apply: true);
        $dhcpd_config = file_get_contents("/var/dhcpd/etc/dhcpd.conf");
        $this->assert_str_does_not_contain(
            $dhcpd_config,
            "option custom-$option->parent_id-$option->id"
        );
    }
}