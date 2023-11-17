<?php

namespace api\tests;

use API\Core\TestCase;
use api\fields\PortField;
use API\Models\FirewallAlias;

class APIFieldsPortFieldTestCase extends TestCase
{
    /**
     * Checks that alias values are accepted when `allow_alias` is set, and rejected when it is not.
     */   
    public function test_allow_alias() {
        # Create a Field object to test with
        $field = new PortField(allow_alias: true);
        
        # Create a port and host alias to test with
        $port_alias = new FirewallAlias(representation_data: ["name" => "PORTALIAS", "type" => "port"]);
        $host_alias = new FirewallAlias(representation_data: ["name" => "HOSTALIAS", "type" => "host"]);
        $port_alias->create();
        $host_alias->create();
        
        # Ensure the port alias is accepted
        $this->assert_does_not_throw(
            callable: function () use ($field, $port_alias) {
                $field->validate_extra($port_alias->name->value);
            }
        );
        
        # Ensure host aliases are not accepted if `allow_alias` is enabled
        $this->assert_throws_response(
            response_id: "PORT_FIELD_INVALID",
            code: 400,
            callable: function () use ($field, $host_alias) {
                $field->validate_extra($host_alias->name->value);
            }
        );
        
        # Ensure port aliases are not allowed if `allow_alias` is not set
        $this->assert_throws_response(
            response_id: "PORT_FIELD_INVALID",
            code: 400,
            callable: function () use ($field, $port_alias) {
                $field->allow_alias = false;
                $field->validate_extra($port_alias->name->value);
            }
        );
        
        # Delete the aliases created for testing
        $host_alias->delete();
        $host_alias->delete();
    }

    /**
     * Checks that port range values are accepted when `allow_range` is set, and rejected when it is not.
     */
    public function test_allow_range() {
        # Create a Field object to test with
        $field = new PortField(allow_range: true);

        # Ensure a port range is accepted
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("123:321");
            }
        );

        # Ensure ranges are not accepted if `allow_range` is not set
        $this->assert_throws_response(
            response_id: "PORT_FIELD_INVALID",
            code: 400,
            callable: function () use ($field) {
                $field->allow_range = false;
                $field->validate_extra("123:321");
            }
        );
    }
    
    /**
     * Checks that single port values are always accepted.
     */
    public function test_alway_allow_port() {
        # Create a Field object to test with
        $field = new PortField();

        # Ensure a port range is accepted
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate_extra("123");
            }
        );
    }
    
    /**
     * Checks that the field's `to_internal()` method correctly converts the representation value to its internal form 
     */
    public function test_to_internal() {
        # Create a Field object to test with
        $field = new PortField();
        
        # Ensure port ranges are stored using their internal range character
        $field->internal_range_char = "_";
        $field->value = "123:321";
        $this->assert_equals($field->to_internal(), "123_321");
        $field->internal_range_char = "-";
        $field->value = "100:102";
        $this->assert_equals($field->to_internal(), "100-102");
        
        # Ensure any other value just uses its representation form internally
        $field->value = "100";
        $this->assert_equals($field->to_internal(), "100");
    }

    /**
     * Checks that the field's `from_internal()` method correctly converts the internal value to its representation form
     */
    public function test_from_internal() {
        # Create a Field object to test with
        $field = new PortField();

        # Ensure port ranges are stored using their internal range character
        $field->internal_range_char = "_";
        $field->from_internal("123_321");
        $this->assert_equals($field->value, "123:321");
        $field->internal_range_char = "-";
        $field->from_internal("100-102");
        $this->assert_equals($field->value, "100:102");

        # Ensure any other value just uses its representation form internally
        $field->from_internal("100");
        $this->assert_equals($field->value, "100");
    }
}
