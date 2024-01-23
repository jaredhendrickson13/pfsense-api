<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\FilterAddressField;
use RESTAPI\Fields\SpecialNetworkField;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\RESTAPISettings;

class APIFieldsSpecialNetworkFieldTestCase extends TestCase
{
    /**
     * Checks that the SpecialNetworkField accepts IP addresses when configured.
     */
    public function test_validate_ipaddr() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_ipaddr: true);
        
        # Ensure IP addresses are accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("127.0.0.1");
                $field->validate_extra("fe80::1");
            }
        );
        
        # Ensure IP addresses are not accepted if not allowed
        $field->allow_ipaddr = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("127.0.0.1");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts allowed_keywords when configured.
     */
    public function test_validate_allowed_keywords() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allowed_keywords: ["allow_this_value", "allow_this_value_too"]);

        # Ensure keywords are accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("allow_this_value");
                $field->validate_extra("allow_this_value_too");
            }
        );

        # Ensure non-keywords are not excepted
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("not_an_allowed_keyword");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts subnets when configured.
     */
    public function test_validate_subnet() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_subnet: true);

        # Ensure subnets are accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("127.0.0.1/24");
                $field->validate_extra("fe80::1/128");
            }
        );

        # Ensure subnets are not accepted if not allowed
        $field->allow_subnet = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("127.0.0.1/24");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts aliases when configured.
     */
    public function test_validate_alias() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_alias: true);
        
        # Create a host, network and port alias
        $host_alias = new FirewallAlias(data: ["name" => "host_alias", "type" => "host"]);
        $network_alias = new FirewallAlias(data: ["name" => "network_alias", "type" => "network"]);
        $port_alias = new FirewallAlias(data: ["name" => "port_alias", "type" => "port"]);
        $host_alias->create();
        $network_alias->create();
        $port_alias->create();

        # Ensure host and network subnets are accepted
        $this->assert_does_not_throw(
            function () use ($field, $host_alias, $network_alias) {
                $field->validate_extra($host_alias->name->value);
                $field->validate_extra("fe80::1/128");
            }
        );
        
        # Ensure port aliases are never accepted
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field, $port_alias) {
                $field->validate_extra($port_alias->name->value);
            }
        );

        # Ensure aliases are not accepted if not allowed
        $field->allow_alias = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field, $host_alias) {
                $field->validate_extra($host_alias->name->value);
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts literal `(self)` when configured.
     */
    public function test_validate_self() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_self: true);

        # Ensure (self) is accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("(self)");
            }
        );

        # Ensure (self) is not accepted if not allowed
        $field->allow_self = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("(self)");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts literal `l2tp` when configured.
     */
    public function test_validate_l2tp() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_l2tp: true);

        # Ensure l2tp is accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("l2tp");
            }
        );

        # Ensure l2tp is not accepted if not allowed
        $field->allow_l2tp = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("l2tp");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts literal `pppoe` when configured.
     */
    public function test_validate_pppoe() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_pppoe: true);

        # Ensure pppoe is accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("pppoe");
            }
        );

        # Ensure pppoe is not accepted if not allowed
        $field->allow_pppoe = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("pppoe");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts interface values when configured.
     */
    public function test_validate_interface() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_interface: true, allow_interface_ip: false);

        # Ensure interface is accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("lan");
            }
        );
        
        # Ensure `interface_ip` values are not allowed even if `interface` is allowed
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("lan:ip");
            }
        );

        # Ensure interface is not accepted if not allowed
        $field->allow_interface = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("lan");
            }
        );
    }

    /**
     * Checks that the SpecialNetworkField accepts interface_ip values when configured.
     */
    public function test_validate_interface_ip() {
        # Create a SpecialNetworkField to test with
        $field = new SpecialNetworkField(allow_interface: false, allow_interface_ip: true);

        # Ensure interface is accepted
        $this->assert_does_not_throw(
            function () use ($field) {
                $field->validate_extra("lan:ip");
            }
        );

        # Ensure `interface` values are not allowed even if `interface_ip` is allowed
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("lan");
            }
        );

        # Ensure interface_ip is not accepted if not allowed
        $field->allow_interface_ip = false;
        $this->assert_throws_response(
            response_id: "SPECIAL_NETWORK_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("lan:ip");
            }
        );
    }
    
    /**
     * Checks that to_internal() correctly converts representation values to their interval values
     */
    public function test_to_internal() {
        # Ensure interface values are converted to their internal values correctly
        $api_settings = new RESTAPISettings();
        $api_settings->represent_interfaces_as->value = "descr";
        $api_settings->update();
        $field = new SpecialNetworkField();
        $field->value = "LAN";
        $this->assert_equals($field->to_internal(), "lan");
        $api_settings->represent_interfaces_as->value = "id";
        $api_settings->update();
        
        # Ensure interface values with the :ip modifer are correctly to converted to their internal values
        $field->value = "lan:ip";
        $this->assert_equals($field->to_internal(), "lanip");
        
        # Ensure any other values just match their representation value
        $field->value = "127.0.0.1";
        $this->assert_equals($field->to_internal(), $field->value);
    }
    
    /**
     * Checks that from_internal() correctly converts internal values into their representation values
     */
    public function test_from_internal() {
        # Ensure interface values are correctly represented
        $api_settings = new RESTAPISettings();
        $api_settings->represent_interfaces_as->value = "descr";
        $api_settings->update();
        $field = new SpecialNetworkField();
        $field->from_internal("lan");
        $this->assert_equals($field->value, "LAN");
        $api_settings->represent_interfaces_as->value = "id";
        $api_settings->update();
        
        # Ensure interface_ip values are correctly represented
        $field = new SpecialNetworkField();
        $field->from_internal("lanip");
        $this->assert_equals($field->value, "lan:ip");
        
        # Ensure any other value is simply represented the same as the internal value
        $field->from_internal("127.0.0.1");
        $this->assert_equals($field->value, "127.0.0.1");
        $field->from_internal("example_alias");
        $this->assert_equals($field->value, "example_alias");
        $field->from_internal("l2tp");
        $this->assert_equals($field->value, "l2tp");
    }

    /**
     * Checks that expected labels are set after running `validated_extra()`.
     */
    public function test_validate_extra_sets_labels(): void {
        # Create a firewall alias we can use to populate the `is_alias` label for this test
        $alias = new FirewallAlias(name: "test_alias", type: "host");
        $alias->create();

        # Create a FilterAddressField to use for testing
        $field = new SpecialNetworkField(required: true, allow_ipaddr: true, allow_subnet: true, allow_alias: true);
        $field->name = "test";

        # Ensure the field obtains the `is_alias` label when the value is an existing firewall alias
        $field->value = $alias->name->value;
        $field->validate();
        $this->assert_equals($field->labels, ["is_alias"]);

        # Ensure the field obtains the `is_ipaddr` and `is_ipaddrv4` labels when the value is an IPv4 address
        $field->value = "1.2.3.4";
        $field->validate();
        $this->assert_equals($field->labels, ["is_ipaddrv4", "is_ipaddr"]);

        # Ensure the field obtains the `is_ipaddr` and `is_ipaddrv6` labels when the value is an IPv4 address
        $field->value = "1234::1";
        $field->validate();
        $this->assert_equals($field->labels, ["is_ipaddrv6", "is_ipaddr"]);

        # Ensure the field obtains the `is_subnet` and `is_subnetv4` labels when the value is an IPv4 address
        $field->value = "1.2.3.4/32";
        $field->validate();
        $this->assert_equals($field->labels, ["is_subnetv4", "is_subnet"]);

        # Ensure the field obtains the `is_subnet` and `is_subnetv6` labels when the value is an IPv4 address
        $field->value = "1234::1/128";
        $field->validate();
        $this->assert_equals($field->labels, ["is_subnetv6", "is_subnet"]);

        # Ensure the field obtains the `is_interface` label when the value is an interface based value
        $field->value = "lan";
        $field->validate();
        $this->assert_equals($field->labels, ["is_interface"]);

        # Delete the firewall alias
        $alias->delete();
    }
}
