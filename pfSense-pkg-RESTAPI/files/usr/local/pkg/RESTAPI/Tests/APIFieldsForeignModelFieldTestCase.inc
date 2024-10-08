<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\RoutingGateway;
use RESTAPI\Models\RoutingGatewayGroup;
use RESTAPI\Models\TrafficShaper;

class APIFieldsForeignModelFieldTestCase extends TestCase {
    /**
     * Ensure assigned `model_name` must be an existing Model class name.
     */
    public function test_model_name_must_be_name_of_model_class(): void {
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_NAME',
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: 'NOT_A_REAL_MODEL', model_field: 'NOT_A_REAL_MODEL_FIELD');
            },
        );
    }

    /**
     * Ensure assigned `model_name` must be an existing Model class name.
     */
    public function test_model_must_be_many(): void {
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_ASSIGNED_NON_MANY_MODEL',
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: 'RESTAPISettings', model_field: 'NOT_A_REAL_MODEL_FIELD');
            },
        );
    }

    /**
     * Ensure assigned `model_field` must be an existing Field name from the assigned Model class.
     */
    public function test_model_field_must_be_existing_field_name(): void {
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD',
            code: 500,
            callable: function () {
                new ForeignModelField(model_name: 'RoutingGateway', model_field: 'NOT_A_REAL_MODEL_FIELD');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: 'RoutingGateway', model_field: 'id');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: 'RoutingGateway', model_field: 'name');
            },
        );
    }

    /**
     * Ensure assigned `model_field_internal` must be an existing Field name from the assigned Model class.
     */
    public function test_model_field_internal_must_be_existing_field_name(): void {
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_WITH_UNKNOWN_MODEL_FIELD_INTERNAL',
            code: 500,
            callable: function () {
                new ForeignModelField(
                    model_name: 'RoutingGateway',
                    model_field: 'id',
                    model_field_internal: 'NOT_A_REAL_MODEL_FIELD',
                );
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: 'RoutingGateway', model_field: 'name', model_field_internal: 'id');
            },
        );
        $this->assert_does_not_throw(
            callable: function () {
                new ForeignModelField(model_name: 'RoutingGateway', model_field: 'id', model_field_internal: 'name');
            },
        );
    }

    /**
     * Ensure the `from_internal()` method correctly locates the correct object by its internal value.
     */
    public function test_from_internal(): void {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(
            data: ['name' => 'HOST_ALIAS', 'type' => 'host', 'address' => []],
            async: false,
        );
        $host_alias->create();
        $net_alias = new FirewallAlias(
            data: ['name' => 'NET_ALIAS', 'type' => 'network', 'address' => []],
            async: false,
        );
        $net_alias->create();
        $port_alias = new FirewallAlias(
            data: ['name' => 'PORT_ALIAS', 'type' => 'port', 'address' => []],
            async: false,
        );
        $port_alias->create();

        # Create a ForeignModelField object for testing
        $field = new ForeignModelField(model_name: 'FirewallAlias', model_field: 'name', model_field_internal: 'id');
        $field->from_internal(1);
        $this->assert_equals($field->value, 'NET_ALIAS');

        # Delete the firewall aliases used for testing
        $host_alias->delete();
        $net_alias->delete();
        $port_alias->delete(apply: true);
    }

    /**
     * Ensure the `to_internal()` method correctly sets the internal value of the assigned object.
     */
    public function test_to_internal(): void {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(
            data: ['name' => 'HOST_ALIAS', 'type' => 'host', 'address' => []],
            async: false,
        );
        $host_alias->create();
        $net_alias = new FirewallAlias(
            data: ['name' => 'NET_ALIAS', 'type' => 'network', 'address' => []],
            async: false,
        );
        $net_alias->create();
        $port_alias = new FirewallAlias(
            data: ['name' => 'PORT_ALIAS', 'type' => 'port', 'address' => []],
            async: false,
        );
        $port_alias->create();

        # Create a ForeignModelField object for testing
        $field = new ForeignModelField(model_name: 'FirewallAlias', model_field: 'id', model_field_internal: 'name');
        $field->value = 2;
        $this->assert_equals($field->to_internal(), 'PORT_ALIAS');

        # Delete the firewall aliases used for testing
        $host_alias->delete();
        $net_alias->delete();
        $port_alias->delete(apply: true);
    }

    /**
     * Ensure the value assigned to the field must relate to existing object
     */
    public function test_value_must_relate_to_foreign_model(): void {
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_VALUE_NOT_FOUND',
            code: 404,
            callable: function () {
                $field = new ForeignModelField(model_name: 'FirewallAlias', model_field: 'name', default: 'test');
                $field->name = 'test_field';
                $field->value = 'NO FIREWALL ALIAS EXISTS WITH THIS NAME';
                $field->validate();
            },
        );
    }

    /**
     * Ensure the `model_query` correctly limits the Model objects this Field can relate to.
     */
    public function test_model_query_limits_objects(): void {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(data: ['name' => 'HOST_ALIAS', 'type' => 'host', 'address' => []]);
        $host_alias->create();
        $net_alias = new FirewallAlias(data: ['name' => 'NET_ALIAS', 'type' => 'network', 'address' => []]);
        $net_alias->create();
        $port_alias = new FirewallAlias(
            data: ['name' => 'PORT_ALIAS', 'type' => 'port', 'address' => []],
            async: false,
        );
        $port_alias->create();

        # Create a ForeignModelField object that can only relate to FirewallAlias objects that are of `type` network
        $field = new ForeignModelField(
            model_name: 'FirewallAlias',
            model_field: 'name',
            model_query: ['type' => 'network'],
            default: 'test',
        );
        $field->name = 'test_field';
        $field->value = 'NET_ALIAS';

        # Ensure an error is not thrown when we try to validate with a value that relates to an existing network type alias
        $this->assert_does_not_throw(
            callable: function () use ($field) {
                $field->validate();
            },
        );

        # Ensure an error is thrown when we try to validate with a value that relates to an existing port type alias
        $field->value = 'PORT_ALIAS';
        $this->assert_throws_response(
            response_id: 'FOREIGN_MODEL_FIELD_VALUE_NOT_FOUND',
            code: 404,
            callable: function () use ($field) {
                $field->validate();
            },
        );

        # Delete the firewall aliases used for testing
        $host_alias->delete();
        $net_alias->delete();
        $port_alias->delete(apply: true);
    }

    /**
     * Ensure the `get_related_model` correctly returns the model object(s) associated with this field's current value(s).
     */
    public function test_get_related_model(): void {
        # Create FirewallAlias objects to use for testing purposes
        $host_alias = new FirewallAlias(
            data: ['name' => 'HOST_ALIAS', 'type' => 'host', 'address' => []],
            async: false,
        );
        $host_alias->create();
        $net_alias = new FirewallAlias(
            data: ['name' => 'NET_ALIAS', 'type' => 'network', 'address' => []],
            async: false,
        );
        $net_alias->create();
        $port_alias = new FirewallAlias(
            data: ['name' => 'PORT_ALIAS', 'type' => 'port', 'address' => []],
            async: false,
        );
        $port_alias->create();

        # Create a non `many` ForeignModelField object
        $field = new ForeignModelField(model_name: 'FirewallAlias', model_field: 'name', default: 'test');
        $field->value = 'NET_ALIAS';

        # Ensure the object returned by `get_related_model()` is the FirewallAlias object with the name `NET_ALIAS`
        $this->assert_equals($field->get_related_model()->name->value, $field->value);

        # Create a `many` ForeignModelField object  and assign it all of the above firewall aliases by name
        $field = new ForeignModelField(model_name: 'FirewallAlias', model_field: 'name', default: 'test', many: true);
        $field->value = ['HOST_ALIAS', 'NET_ALIAS', 'PORT_ALIAS'];

        # Ensure the object returned by `get_related_model()` is a ModelSet
        $this->assert_is_true($field->get_related_models()->exists());

        # Ensure each model object within the ModelSet is a FirewallAlias object that corresponds to this Fields values
        foreach ($field->get_related_models()->model_objects as $alias) {
            $this->assert_is_true(in_array($alias->name->value, $field->value));
        }

        # Delete the firewall aliases used for testing
        $host_alias->delete();
        $net_alias->delete();
        $port_alias->delete(apply: true);
    }

    /**
     * Checks that we can use Model classes that have parent classes assigned.
     */
    public function test_foreign_models_with_parents(): void {
        # Use TrafficShapers to test this
        $shaper1 = new TrafficShaper(
            interface: 'lan',
            scheduler: 'FAIRQ',
            bandwidth: 100,
            bandwidthtype: 'Mb',
            queue: [['name' => 'TestQueue1', 'qlimit' => 50, 'bandwidth' => 50, 'bandwidthtype' => 'Mb']],
            async: false,
        );
        $shaper1->create();
        $shaper2 = new TrafficShaper(
            interface: 'wan',
            scheduler: 'FAIRQ',
            bandwidth: 100,
            bandwidthtype: 'Mb',
            queue: [['name' => 'TestQueue2', 'qlimit' => 50, 'bandwidth' => 50, 'bandwidthtype' => 'Mb']],
            async: false,
        );
        $shaper2->create(apply: true);

        # Define a ForeignModelField that targets DNSResolverHostOverrideAliases
        $field = new ForeignModelField(model_name: 'TrafficShaperQueue', model_field: 'name');

        # Ensure we are able to obtain the shaper queue with `name` value set to `TestQueue1`
        $field->value = 'TestQueue1';
        $this->assert_equals($field->value, $field->get_related_model()->name->value);

        # Ensure we are able to obtain many models
        $field->many = true;
        $field->value = ['TestQueue2', 'TestQueue1'];
        $related_objects = $field->get_related_models();
        $this->assert_equals($related_objects->model_objects[0]->name->value, 'TestQueue2');
        $this->assert_equals($related_objects->model_objects[1]->name->value, 'TestQueue1');

        # Delete the shapers
        $shaper1->delete();
        $shaper2->delete(apply: true);
    }

    /**
     * Checks that values specified in `allowed_keywords` are always accepted.
     */
    public function test_allowed_keywords(): void {
        # Ensure no error is thrown if we validate a ForeignModelField assigned an allowed keyword.
        $this->assert_does_not_throw(
            callable: function () {
                $field = new ForeignModelField(
                    model_name: 'FirewallAlias',
                    model_field: 'name',
                    allowed_keywords: ['always_accept_this_value'],
                    required: true,
                );
                $field->name = 'test';
                $field->value = 'always_accept_this_value';
                $field->validate();
            },
        );
    }

    /**
     * Checks that multiple $model_name classes can be checked for the same $model_field.
     */
    public function test_multiple_model_names(): void {
        # Create a Gateway and GatewayGroup object to use for testing purposes
        $gateway = new RoutingGateway(
            name: 'test_gw',
            interface: 'wan',
            ipprotocol: 'inet',
            gateway: '1.1.1.1',
            nonlocalgateway: true,
        );
        $gateway->create();
        $gateway_group = new RoutingGatewayGroup(
            name: 'test_gw_group',
            trigger: 'down',
            descr: 'Test Gateway Group',
            priorities: [['gateway' => 'test_gw', 'tier' => 1, 'virtual_ip' => 'address']],
        );
        $gateway_group->create();

        # Create a ForeignModelField object for testing
        $field = new ForeignModelField(model_name: ['RoutingGateway', 'RoutingGatewayGroup'], model_field: 'name');
        $field->value = 'test_gw';
        $this->assert_equals($field->get_related_model()->get_class_shortname(), 'RoutingGateway');
        $field->value = 'test_gw_group';
        $this->assert_equals($field->get_related_model()->get_class_shortname(), 'RoutingGatewayGroup');

        # Delete the gateway and gateway group used for testing
        $gateway_group->delete();
        $gateway->delete();
    }
}
