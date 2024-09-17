<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Core\ModelSet;
use RESTAPI\Models\DNSResolverHostOverride;
use RESTAPI\Models\FirewallAlias;

/**
 * Defines test case for testing RESTAPI\Core\ModelSet.
 */
class APICoreModelSetTestCase extends RESTAPI\Core\TestCase {
    /**
     * Checks that a ServerError is thrown in a ModelSet is assigned non-Model objects during construction.
     */
    public function test_throws_server_error_with_non_models(): void {
        $this->assert_throws_response(
            response_id: 'MODEL_SET_CONTAINS_NON_MODEL',
            code: 500,
            callable: function () {
                new ModelSet(model_objects: ['Not a Model!']);
            },
        );
    }

    /**
     * Checks that the ModelSet's `to_representation()` method correctly returns the representations of all
     * contained Model objects.
     */
    public function test_modelset_to_representation(): void {
        # Use the FirewallAlias model to test with
        $test_model1 = new FirewallAlias(data: ['name' => 'test_model1', 'type' => 'port', 'address' => [80]]);
        $test_model2 = new FirewallAlias(
            data: ['name' => 'test_model2', 'type' => 'host', 'address' => ['example.com']],
        );

        # Create a ModelSet using the previously created Models
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2]);

        # Ensure the representation of the ModelSet contains the representations of both objects
        $this->assert_is_true($test_modelset->to_representation()[0] === $test_model1->to_representation());
        $this->assert_is_true($test_modelset->to_representation()[1] === $test_model2->to_representation());
    }

    /**
     * Checks that ModelSet queries using exact matches works as expected.
     */
    public function test_query_exact_match(): void {
        # Create two Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_bool' => true]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_bool' => false]);

        # Add the Models to a ModelSet
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2]);
        $queried_modelset = $test_modelset->query(['test_bool__exact' => true]);

        # Ensure the query only returned Models with `test_bool` set to `true`.
        $this->assert_equals(count($queried_modelset->model_objects), 1);
        $this->assert_equals($queried_modelset->first(), $test_model1);

        # Ensure the 'exact' query filter is assumed as the default filter if none was specified
        $this->assert_equals(
            $queried_modelset->model_objects,
            $test_modelset->query(['test_bool' => true])->model_objects,
        );
    }

    /**
     * Checks that ModelSet queries using except matches works as expected.
     */
    public function test_query_except(): void {
        # Create two Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_bool' => true]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_bool' => false]);

        # Add the Models to a ModelSet
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2]);
        $queried_modelset = $test_modelset->query(['test_bool__except' => true]);

        # Ensure the query only returned Models with `test_bool` not set to true
        $this->assert_equals(count($queried_modelset->model_objects), 1);
        $this->assert_equals($queried_modelset->model_objects[0], $test_model2);
    }

    /**
     * Checks that ModelSet queries using the 'startswith' filer works as expected.
     */
    public function test_query_startswith(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_string_unique' => "Don't find me!"]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_string_unique' => 'Find me!']);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_string_unique' => "Don't find me!"]);

        # Add the Models to a ModelSet and query for Models with `test_string_unique` values starting with 'Find'
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3]);
        $queried_modelset = $test_modelset->query(['test_string_unique__startswith' => 'Find']);

        # Ensure the query only returned Models with `test_string_unique` values starting with 'Find'.
        $this->assert_equals(count($queried_modelset->model_objects), 1);
        $this->assert_equals($queried_modelset->model_objects[0], $test_model2);
    }

    /**
     * Checks that ModelSet queries using the 'endswith' filer works as expected.
     */
    public function test_query_endswith(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_string_unique' => 'I match!']);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_string_unique' => "I don't!"]);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_string_unique' => 'I match!']);

        # Add the Models to a ModelSet and query for Models with `test_string_unique` values ending with 'match!'
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3]);
        $queried_modelset = $test_modelset->query(['test_string_unique__endswith' => 'match!']);

        # Ensure the query only returned Models with `test_string_unique` values ending with 'match!'.
        $this->assert_equals(count($queried_modelset->model_objects), 2);
        $this->assert_equals($queried_modelset->model_objects, [$test_model1, $test_model3]);
    }

    /**
     * Checks that ModelSet queries using the 'contains' filer works as expected.
     */
    public function test_query_contains(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_string_unique' => 'I match!']);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_string_unique' => 'I match!']);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_string_unique' => "I don't!"]);

        # Add the Models to a ModelSet and query for Models with `test_string_unique` values containing  'match'
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3]);
        $queried_modelset = $test_modelset->query(['test_string_unique__contains' => 'match']);

        # Ensure the query only returned Models with `test_string_unique` values containing 'match'.
        $this->assert_equals(count($queried_modelset->model_objects), 2);
        $this->assert_equals($queried_modelset->model_objects, [$test_model1, $test_model2]);
    }

    /**
     * Checks that ModelSet queries using the 'lt' (less than) filer works as expected.
     */
    public function test_query_lt(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_integer' => 10]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_integer' => 30]);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_integer' => 20]);

        # Add the Models to a ModelSet and query for Models with `test_integer` values less than 30
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3]);
        $queried_modelset = $test_modelset->query(['test_integer__lt' => 30]);

        # Ensure the query only returned Models with `test_integer` values less than 30
        $this->assert_equals(count($queried_modelset->model_objects), 2);
        $this->assert_equals($queried_modelset->model_objects, [$test_model1, $test_model3]);
    }

    /**
     * Checks that ModelSet queries using the 'lte' (less than or equal to) filter works as expected.
     */
    public function test_query_lte(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_integer' => 10]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_integer' => 31]);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_integer' => 20]);
        $test_model4 = new RESTAPI\Models\Test(data: ['test_integer' => 30]);

        # Add the Models to a ModelSet and query for Models with `test_integer` values less than or equal to 30
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3, $test_model4]);
        $queried_modelset = $test_modelset->query(['test_integer__lte' => 30]);

        # Ensure the query only returned Models with `test_integer` values less than or equal to 30
        $this->assert_equals(count($queried_modelset->model_objects), 3);
        $this->assert_equals($queried_modelset->model_objects, [$test_model1, $test_model3, $test_model4]);
    }

    /**
     * Checks that ModelSet queries using the 'gt' (greater than) filer works as expected.
     */
    public function test_query_gt(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_integer' => 10]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_integer' => 30]);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_integer' => 20]);

        # Add the Models to a ModelSet and query for Models with `test_integer` values greater than 10
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3]);
        $queried_modelset = $test_modelset->query(['test_integer__gt' => 10]);

        # Ensure the query only returned Models with `test_integer` values greater than 30
        $this->assert_equals(count($queried_modelset->model_objects), 2);
        $this->assert_equals($queried_modelset->model_objects, [$test_model2, $test_model3]);
    }

    /**
     * Checks that ModelSet queries using the 'gte' (greater than or equal to) filter works as expected.
     */
    public function test_query_gte(): void {
        # Create three Models with unique values to add to the ModelSet
        $test_model1 = new RESTAPI\Models\Test(data: ['test_integer' => 10]);
        $test_model2 = new RESTAPI\Models\Test(data: ['test_integer' => 5]);
        $test_model3 = new RESTAPI\Models\Test(data: ['test_integer' => 20]);
        $test_model4 = new RESTAPI\Models\Test(data: ['test_integer' => 30]);

        # Add the Models to a ModelSet and query for Models with `test_integer` values greater than or equal to 10
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2, $test_model3, $test_model4]);
        $queried_modelset = $test_modelset->query(['test_integer__gte' => 10]);

        # Ensure the query only returned Models with `test_integer` values greater than or equal to 10
        $this->assert_equals(count($queried_modelset->model_objects), 3);
        $this->assert_equals($queried_modelset->model_objects, [$test_model1, $test_model3, $test_model4]);
    }

    /**
     * Checks that the ModelSet queries using the `has` (checks if an array field has a given value) filter works
     * as expected.
     */
    public function test_query_array_contains(): void {
        # Create firewall alias objects to test with
        $alias_a = new FirewallAlias(data: ['name' => 'alias_a', 'type' => 'host', 'address' => ['a.co']]);
        $alias_b = new FirewallAlias(data: ['name' => 'alias_b', 'type' => 'host', 'address' => ['b.co']]);
        $alias_a->create();
        $alias_b->create();

        # Add the aliases to a ModelSet
        $modelset = new ModelSet(model_objects: [$alias_a, $alias_b]);

        # Ensure the `has` query correctly identifies the targetted object(s)
        $this->assert_equals($modelset->query(address__contains: 'b.co')->first()->name->value, $alias_b->name->value);
        $this->assert_equals($modelset->query(address__contains: 'a.co')->first()->name->value, $alias_a->name->value);

        # Delete the firewall aliases
        $alias_a->delete();
        $alias_b->delete(apply: true);
    }

    /**
     * Checks that the ModelSet exists() method correctly identifies when Model objects are set.
     */
    public function test_exists(): void {
        $test_model = new RESTAPI\Models\Test();

        # Ensure exists() returns true when at least one Model is assigned
        $test_modelset = new ModelSet(model_objects: [$test_model]);
        $this->assert_is_true($test_modelset->exists());

        # Ensure exists() returns false when no Models are assigned
        $test_modelset = new ModelSet(model_objects: []);
        $this->assert_is_false($test_modelset->exists());
    }

    /**
     * Checks that the ModelSet count() method correctly identifies the number of Model objects set.
     */
    public function test_count(): void {
        $test_model1 = new RESTAPI\Models\Test();
        $test_model2 = new RESTAPI\Models\Test();

        # Ensure count() returns the correct number of Model objects assigned
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2]);
        $this->assert_equals($test_modelset->count(), 2);
    }

    /**
     * Checks that the ModelSet first() method correctly identifies the first Model object set.
     */
    public function test_first(): void {
        $test_model1 = new RESTAPI\Models\Test();
        $test_model2 = new RESTAPI\Models\Test();

        # Ensure first() returns the first object when at least one Model is assigned
        $test_modelset = new ModelSet(model_objects: [$test_model1, $test_model2]);
        $this->assert_equals($test_modelset->first(), $test_model1);

        # Ensure an error is thrown if first() is called without any model objects assigned
        $this->assert_throws_response(
            response_id: 'MODELSET_FIRST_REQUESTED_WITH_NO_MODEL_OBJECTS',
            code: 500,
            callable: function () {
                $test_modelset = new ModelSet(model_objects: []);
                $test_modelset->first();
            },
        );
    }

    /**
     * Checks that the 'delete()' method correctly deletes all Model objects in the ModelSet
     */
    public function test_delete_many(): void {
        # Create FirewallAlias models
        $host_alias_0 = new FirewallAlias(name: 'host0', type: 'host');
        $host_alias_0->create();
        $host_alias_1 = new FirewallAlias(name: 'host1', type: 'host');
        $host_alias_1->create();
        $host_alias_2 = new FirewallAlias(name: 'host2', type: 'host');
        $host_alias_2->create();

        # Delete all the aliases in the ModelSet
        $modelset = new ModelSet([$host_alias_0, $host_alias_1, $host_alias_2]);
        $deleted_aliases = $modelset->delete();

        # Ensure all aliases were deleted
        $this->assert_equals($deleted_aliases->count(), 3);
        $this->assert_is_false(FirewallAlias::query(type: 'host')->exists());
    }

    /**
     * Checks that the 'reverse()' method correctly reverses the order of the Model objects in the ModelSet.
     */
    public function test_reverse(): void {
        # Create three firewall aliases
        $alias_a = new FirewallAlias(name: 'alias_a', type: 'host');
        $alias_b = new FirewallAlias(name: 'alias_b', type: 'host');
        $alias_c = new FirewallAlias(name: 'alias_c', type: 'host');
        $alias_a->create();
        $alias_b->create();
        $alias_c->create();

        # Read all firewall aliases and ensure they are in the correct (normal) order
        $modelset = FirewallAlias::read_all();
        $this->assert_equals($modelset->model_objects[0]->name->value, 'alias_a');
        $this->assert_equals($modelset->model_objects[0]->id, 0);
        $this->assert_equals($modelset->model_objects[1]->name->value, 'alias_b');
        $this->assert_equals($modelset->model_objects[1]->id, 1);
        $this->assert_equals($modelset->model_objects[2]->name->value, 'alias_c');
        $this->assert_equals($modelset->model_objects[2]->id, 2);

        # Reverse the order of the firewall aliases and ensure they are in the correct (reversed) order
        $reversed_modelset = $modelset->reverse();
        $this->assert_equals($reversed_modelset->model_objects[0]->name->value, 'alias_c');
        $this->assert_equals($reversed_modelset->model_objects[0]->id, 2);
        $this->assert_equals($reversed_modelset->model_objects[1]->name->value, 'alias_b');
        $this->assert_equals($reversed_modelset->model_objects[1]->id, 1);
        $this->assert_equals($reversed_modelset->model_objects[2]->name->value, 'alias_a');
        $this->assert_equals($reversed_modelset->model_objects[2]->id, 0);

        # Delete the firewall aliases
        $alias_a->delete();
        $alias_b->delete();
        $alias_c->delete();
    }

    /**
     * Checks that the sort() method correctly sorts the Model objects in the ModelSet.
     */
    public function test_sort(): void {
        # Add three firewall aliases to a ModelSet
        $alias_b = new FirewallAlias(name: 'alias_b', type: 'port');
        $alias_a = new FirewallAlias(name: 'alias_a', type: 'host');
        $alias_c = new FirewallAlias(name: 'alias_c', type: 'network');
        $modelset = new ModelSet([$alias_a, $alias_b, $alias_c]);

        # Sort the ModelSet by the 'name' field in ascending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['name'])->model_objects,
            (new ModelSet([$alias_a, $alias_b, $alias_c]))->model_objects,
        );

        # Sort the ModelSet by the 'name' field in descending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['name'], order: SORT_DESC)->model_objects,
            (new ModelSet([$alias_c, $alias_b, $alias_a]))->model_objects,
        );

        # Sort the ModelSet by the 'type' field in ascending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['type'])->model_objects,
            (new ModelSet([$alias_a, $alias_c, $alias_b]))->model_objects,
        );

        # Sort the ModelSet by the 'type' field in descending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['type'], order: SORT_DESC)->model_objects,
            (new ModelSet([$alias_b, $alias_c, $alias_a]))->model_objects,
        );
    }

    /**
     * Ensures multi-field sorts work as expected.
     */
    public function test_sort_multiple_fields(): void
    {
        # Add DNS Resolver host overrides to test this
        $host_1 = new DNSResolverHostOverride(host: 'a', domain: 'a.example.com', ip: ['127.0.0.1']);
        $host_2 = new DNSResolverHostOverride(host: 'b', domain: 'a.example.com', ip: ['127.35.0.2']);
        $host_3 = new DNSResolverHostOverride(host: 'c', domain: 'a.example.com', ip: ['127.0.8.3']);
        $host_4 = new DNSResolverHostOverride(host: 'a', domain: 'b.example.com', ip: ['127.0.1.4']);
        $host_5 = new DNSResolverHostOverride(host: 'b', domain: 'b.example.com', ip: ['127.5.1.7']);
        $host_6 = new DNSResolverHostOverride(host: 'c', domain: 'b.example.com', ip: ['127.3.3.3']);
        $host_7 = new DNSResolverHostOverride(host: 'a', domain: 'c.example.com', ip: ['127.0.4.2']);
        $host_8 = new DNSResolverHostOverride(host: 'b', domain: 'c.example.com', ip: ['127.3.0.1']);
        $host_9 = new DNSResolverHostOverride(host: 'c', domain: 'c.example.com', ip: ['127.5.1.3']);

        # Create a ModelSet using the previously created Models in a random order
        $modelset = new ModelSet([$host_1, $host_2, $host_3, $host_4, $host_5, $host_6, $host_7, $host_8, $host_9]);
        shuffle($modelset->model_objects);

        # Sort the ModelSet by the host and domain fields in ascending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['host', 'domain'])->model_objects,
            [$host_1, $host_4, $host_7, $host_2, $host_5, $host_8, $host_3, $host_6, $host_9],
        );

        # Sort the ModelSet by the domain and host fields in descending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['domain', 'host'], order: SORT_DESC)->model_objects,
            [$host_9, $host_8, $host_7, $host_6, $host_5, $host_4, $host_3, $host_2, $host_1],
        );

        # Sort the ModelSet by domain and ip fields in ascending order and ensure the order is correct
        $this->assert_equals(
            $modelset->sort(['domain', 'ip'])->model_objects,
            [$host_1, $host_3, $host_2, $host_4, $host_6, $host_5, $host_7, $host_8, $host_9],
        );
    }
}
