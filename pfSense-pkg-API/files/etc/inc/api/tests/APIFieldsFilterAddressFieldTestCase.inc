<?php

namespace api\tests;

use API\Core\TestCase;
use API\Fields\FilterAddressField;
use API\Models\FirewallAlias;

class APIFieldsFilterAddressFieldTestCase extends TestCase
{
    /**
     * Checks that IP address values are allowed when `allow_ipaddr` is set.
     */
    public function test_allow_ipaddr() {
        # Create field object to test with
        $field = new FilterAddressField(allow_ipaddr: true);
        
        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("127.0.0.1");
            }
        );
        
        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_ipaddr = false;
                $field->validate_extra("127.0.0.1");
            }
        );
    }

    /**
     * Checks that subnet values are allowed when `allow_subnet` is set.
     */
    public function test_allow_subnet() {
        # Create field object to test with
        $field = new FilterAddressField(allow_subnet: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("127.0.0.1/24");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_subnet = false;
                $field->validate_extra("127.0.0.1/24");
            }
        );
    }

    /**
     * Checks that alias values are allowed when `allow_alias` is set.
     */
    public function test_allow_alias() {
        # Create a host, network and port alias
        $host_alias = new FirewallAlias(representation_data: ["name" => "HOSTALIAS", "type" => "host"]);
        $host_alias->create();
        $net_alias = new FirewallAlias(representation_data: ["name" => "NETALIAS", "type" => "network"]);
        $net_alias->create();
        $port_alias = new FirewallAlias(representation_data: ["name" => "PORTALIAS", "type" => "port"]);
        $port_alias->create();

        # Create field object to test with
        $field = new FilterAddressField(allow_alias: true);

        # Ensure an error is not thrown when the value is allowed (host and network alias names)
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("HOSTALIAS");
            }
        );
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("NETALIAS");
            }
        );
        
        # Ensure the port aliases are never allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("PORTALIAS");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $field->allow_alias = false;
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("HOSTALIAS");
            }
        );
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->validate_extra("NETALIAS");
            }
        );
        
        # Delete aliases used by this test
        $host_alias->delete();
        $net_alias->delete();
        $port_alias->delete();
    }

    /**
     * Checks that interface values are allowed when `allow_interface` is set.
     */
    public function test_allow_interface() {
        # Create field object to test with
        $field = new FilterAddressField(allow_interface: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("wan");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_interface = false;
                $field->validate_extra("wan");
            }
        );
    }

    /**
     * Checks that any values are allowed when `allow_any` is set.
     */
    public function test_allow_any() {
        # Create field object to test with
        $field = new FilterAddressField(allow_any: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("any");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_any = false;
                $field->validate_extra("any");
            }
        );
    }

    /**
     * Checks that (self) values are allowed when `allow_self` is set.
     */
    public function test_allow_self() {
        # Create field object to test with
        $field = new FilterAddressField(allow_self: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("(self)");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_self = false;
                $field->validate_extra("(self)");
            }
        );
    }

    /**
     * Checks that l2tp values are allowed when `allow_l2tp` is set.
     */
    public function test_allow_l2tp() {
        # Create field object to test with
        $field = new FilterAddressField(allow_l2tp: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("l2tp");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_l2tp = false;
                $field->validate_extra("l2tp");
            }
        );
    }

    /**
     * Checks that pppoe values are allowed when `allow_pppoe` is set.
     */
    public function test_allow_pppoe() {
        # Create field object to test with
        $field = new FilterAddressField(allow_pppoe: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("pppoe");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_pppoe = false;
                $field->validate_extra("pppoe");
            }
        );
    }

    /**
     * Checks that inverted values are allowed when `allow_invert` is set.
     */
    public function test_allow_invert() {
        # Create field object to test with
        $field = new FilterAddressField(allow_invert: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("!127.0.0.1");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_invert = false;
                $field->validate_extra("!127.0.0.1");
            }
        );
        
        # Ensure `any` values cannot use the invert prefix even if invert is allowed
        $this->assert_throws_response(
            response_id: "FILTER_RULE_ADDRESS_FIELD_WITH_INVERT_ANY",
            code: 400,
            callable: function () use ($field) {
                $field->allow_invert = true;
                $field->validate_extra("!any");
            }
        );
    }

    /**
     * Checks that the interface IP modifier (`:ip`) is allowed when `allow_interface_ip` is set.
     */
    public function test_allow_interface_ip_modifier() {
        # Create field object to test with
        $field = new FilterAddressField(allow_interface_ip: true);

        # Ensure an error is not thrown when the value is allowed
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("wan:ip");
            }
        );

        # Ensure an error is thrown when the value is not allowed
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_interface_ip = false;
                $field->validate_extra("wan:ip");
            }
        );

        # Ensure non interface values cannot use the :ip modifier even if allow_interface_ip is set
        $this->assert_throws_response(
            response_id: "FILTER_ADDRESS_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_interface_ip = true;
                $field->validate_extra("pppoe:ip");
            }
        );
    }
    
    /**
     * Checks that the `to_internal()` method correctly represents the field's value as it's internal array
     */
    public function test_to_internal() {
        # Create a field to test with
        $field = new FilterAddressField(required: true);
        $field->name = "test";
        
        # Ensure `any` is properly set internally
        $field->value = "any";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["any" => true]);
        
        # Ensure inverted addresses are properly set internally
        $field->value = "!127.0.0.1";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["not" => true, "address" => "127.0.0.1"]);
        
        # Ensure interface IP values are properly set internally
        $field->value = "wan:ip";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["network" => "wanip"]);
        
        # Ensure alias values are nested under the internal `address` key
        $host_alias = new FirewallAlias(representation_data: ["name" => "HOSTALIAS", "type" => "host"]);
        $host_alias->create(apply: true);
        $field->value = "HOSTALIAS";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["address" => "HOSTALIAS"]);
        $host_alias->delete();

        # Ensure single IP values are nested under the internal `address` key
        $field->value = "127.0.0.1";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["address" => "127.0.0.1"]);

        # Ensure subnet values are nested under the internal `address` key
        $field->value = "127.0.0.1/24";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["address" => "127.0.0.1/24"]);

        # Ensure interface values are nested under the internal `network` key
        $field->value = "wan";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["network" => "wan"]);

        # Ensure l2tp values are nested under the internal `network` key
        $field->value = "l2tp";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["network" => "l2tp"]);

        # Ensure pppoe values are nested under the internal `network` key
        $field->value = "pppoe";
        $field->validate();
        $this->assert_equals($field->to_internal(), ["network" => "pppoe"]);
    }
    
    /**
     * Check that the `from_internal()` method correctly converts the internal value back to its represented
     * value.
     */
    public function test_from_internal() {
        # Create a field to test with
        $field = new FilterAddressField();
        
        # Ensure the internal values with the `not` flag contain the invert prefix
        $field->from_internal(["not" => true, "address" => "127.0.0.1"]);
        $this->assert_equals($field->value, "!127.0.0.1");

        # Ensure the internal values with the interface ip modifier obtain the `:ip` modifier
        $field->from_internal(["network" => "wanip"]);
        $this->assert_equals($field->value, "wan:ip");
        
        # Ensure internal values with the `any` flag are simply represented as `any`
        $field->from_internal(["any" => true]);
        $this->assert_equals($field->value, "any");
    }
}
