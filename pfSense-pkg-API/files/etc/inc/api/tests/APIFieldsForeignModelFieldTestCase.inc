<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Fields\ForeignModelField;
use API\Models\FirewallAlias;

class APIFieldsForeignModelFieldTestCase extends TestCase
{
    /**
     * Ensure assigned `model_name` must be an existing Model class name.
     */
    public function test_model_name_must_be_name_of_model_class() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_NAME",
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: "NOT_A_REAL_MODEL", model_field: "NOT_A_REAL_MODEL_FIELD");
            }
        );
    }

    /**
     * Ensure assigned `model_name` must be an existing Model class name.
     */
    public function test_model_must_be_many() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_ASSIGNED_NON_MANY_MODEL",
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: "APISettings", model_field: "NOT_A_REAL_MODEL_FIELD");
            }
        );
    }

    /**
     * Ensure assigned `model_field` must be an existing Field name from the assigned Model class.
     */
    public function test_model_field_must_be_existing_field_name() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD",
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: "RoutingGateway", model_field: "NOT_A_REAL_MODEL_FIELD");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: "RoutingGateway", model_field: "id");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: "RoutingGateway", model_field: "name");
            }
        );
    }

    /**
     * Ensure assigned `model_field` must be a `unique` Field
     */
    public function test_model_field_must_be_unique() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_WITH_NON_UNIQUE_MODEL_FIELD",
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: "RoutingGateway", model_field: "descr");
            }
        );
    }

    /**
     * Ensure assigned `model_field_internal` must be an existing Field name from the assigned Model class.
     */
    public function test_model_field_internal_must_be_existing_field_name() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD_INTERNAL",
            code: 500,
            callable: function () {
                new ForeignModelField(
                    model_name: "RoutingGateway",
                    model_field: "id",
                    model_field_internal: "NOT_A_REAL_MODEL_FIELD"
                );
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(
                    model_name: "RoutingGateway",
                    model_field: "name",
                    model_field_internal: "id");
            }
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(
                    model_name: "RoutingGateway",
                    model_field: "id",
                    model_field_internal: "name"
                );
            }
        );
    }

    /**
     * Ensure assigned `model_field_internal` must be a `unique` Field
     */
    public function test_model_field_internal_must_be_unique() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_WITH_NON_UNIQUE_MODEL_FIELD_INTERNAL",
            code: 500,
            callable: function () {
                new ForeignModelField(
                    model_name: "RoutingGateway",
                    model_field: "id",
                    model_field_internal: "descr"
                );
            }
        );
    }
    
    /**
     * Ensure the `from_internal()` method correctly locates the correct object by its internal value.
     */
    public function test_from_internal() {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(data: ["name" => "HOST_ALIAS", "type" => "host", "address" => []]);
        $host_alias->create();
        $net_alias = new FirewallAlias(data: ["name" => "NET_ALIAS", "type" => "network", "address" => []]);
        $net_alias->create();
        $port_alias = new FirewallAlias(data: ["name" => "PORT_ALIAS", "type" => "port", "address" => []]);
        $port_alias->create();
        
        # Create a ForeignModelField object for testing
        $field = new ForeignModelField(model_name: "FirewallAlias", model_field: "name", model_field_internal: "id");
        $field->from_internal(1);
        $this->assert_equals($field->value, "NET_ALIAS");
    }

    /**
     * Ensure the `to_internal()` method correctly sets the internal value of the assigned object.
     */
    public function test_to_internal() {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(data: ["name" => "HOST_ALIAS", "type" => "host", "address" => []]);
        $host_alias->create();
        $net_alias = new FirewallAlias(data: ["name" => "NET_ALIAS", "type" => "network", "address" => []]);
        $net_alias->create();
        $port_alias = new FirewallAlias(data: ["name" => "PORT_ALIAS", "type" => "port", "address" => []]);
        $port_alias->create();

        # Create a ForeignModelField object for testing
        $field = new ForeignModelField(model_name: "FirewallAlias", model_field: "id", model_field_internal: "name");
        $field->value = 2;
        $this->assert_equals($field->to_internal(), "PORT_ALIAS");
    }
    
    /**
     * Ensure the value assigned to the field must relate to existing object
     */
    public function test_value_must_relate_to_foreign_model() {
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_VALUE_NOT_FOUND",
            code: 404,
            callable: function () {
                $field = new ForeignModelField(model_name: "FirewallAlias", model_field: "name", default: "test");
                $field->name = "test_field";
                $field->value = "NO FIREWALL ALIAS EXISTS WITH THIS NAME";
                $field->validate();

            }
        );
    }

    /**
     * Ensure the `model_query` correctly limits the Model objects this Field can relate to.
     */
    public function test_model_query_limits_objects() {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(data: ["name" => "HOST_ALIAS", "type" => "host", "address" => []]);
        $host_alias->create();
        $net_alias = new FirewallAlias(data: ["name" => "NET_ALIAS", "type" => "network", "address" => []]);
        $net_alias->create();
        $port_alias = new FirewallAlias(data: ["name" => "PORT_ALIAS", "type" => "port", "address" => []]);
        $port_alias->create();

        # Create a ForeignModelField object that can only relate to FirewallAlias objects that are of `type` netowrk
        $field = new ForeignModelField(
            model_name: "FirewallAlias", 
            model_field: "name", 
            model_query: ["type" => "network"],
            default: "test"
        );
        $field->name = "test_field";
        $field->value = "NET_ALIAS";
        
        # Ensure an error is not thrown when we try to validate with a value that relates to an existing network type alias
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate();
            }
        );
        
        # Ensure an error is thrown when we try to validate with a value that relates to an existing port type alias
        $field->value = "PORT_ALIAS";
        $this->assert_throws_response(
            response_id: "FOREIGN_MODEL_FIELD_VALUE_NOT_FOUND",
            code: 404,
            callable: function () use ($field) {
                $field->validate();
            }
        );
    }
}
