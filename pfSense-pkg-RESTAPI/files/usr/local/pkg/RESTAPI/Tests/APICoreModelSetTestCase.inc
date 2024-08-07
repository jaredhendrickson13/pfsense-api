<?php

namespace RESTAPI\Tests;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI;
use RESTAPI\Core\ModelSet;
use RESTAPI\Models\FirewallAlias;

/**
 * Defines test case for testing RESTAPI\Core\ModelSet.
 */
class APICoreModelSetTestCase extends RESTAPI\Core\TestCase {
    /**
     * Checks that a ServerError is thrown in a ModelSet is assigned non-Model objects during construction.
     */
    public function test_throws_server_error_with_non_models() {
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
    public function test_modelset_to_representation() {
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
    public function test_query_exact_match() {
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
    public function test_query_except() {
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
    public function test_query_startswith() {
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
    public function test_query_endswith() {
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
    public function test_query_contains() {
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
    public function test_query_lt() {
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
    public function test_query_lte() {
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
    public function test_query_gt() {
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
    public function test_query_gte() {
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
    public function test_query_array_contains() {
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
    public function test_exists() {
        $test_model = new RESTAPI\Models\Test();

        # Ensure exists() returns true when at least one Model is assigned
        $test_modelset = new ModelSet(model_objects: [$test_model]);
        $this->assert_is_true($test_modelset->exists());

        # Ensure exists() returns false when no Models are assigned
        $test_modelset = new ModelSet(model_objects: []);
        $this->assert_is_false($test_modelset->exists());
    }

    /**
     * Checks that the ModelSet first() method correctly identifies the first Model object set.
     */
    public function test_first() {
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
}
