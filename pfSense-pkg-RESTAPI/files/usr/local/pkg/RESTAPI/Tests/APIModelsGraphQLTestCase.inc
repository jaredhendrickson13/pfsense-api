<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\GraphQL;

class APIModelsGraphQLTestCase extends TestCase {
    /**
     * Set up FirewallAlias models for testing.
     */
    public function setup(): void {
        # First, create several new FirewallAlias models to query.
        $alias = new FirewallAlias();
        $alias->replace_all([
            [
                'name' => 'test1',
                'type' => 'host',
                'descr' => 'Test alias 1',
                'address' => ['1.2.3.4'],
                'detail' => ['test detail 1'],
            ],
            [
                'name' => 'test2',
                'type' => 'network',
                'descr' => 'Test alias 2',
                'address' => ['1.2.3.4/32'],
                'detail' => ['test detail 2'],
            ],
            [
                'name' => 'test3',
                'type' => 'port',
                'descr' => 'Test alias 3',
                'address' => ['80', '443'],
                'detail' => ['test detail 3'],
            ],
        ]);
    }

    /**
     * Clean up FirewallAlias models after testing.
     */
    public function teardown(): void {
        # Clean up the test data.
        $alias = new FirewallAlias();
        $alias->delete_all();
    }

    /**
     * Check that a GraphQL query to read individual models works as expected.
     */
    public function test_graphql_read(): void {
        # Define a new GraphQL model to execute a read query against.
        $graphql = new GraphQL();
        $graphql->query->value = '{ readFirewallAlias(id: 0) { name type descr address detail } }';
        $graphql->create();

        # Ensure the result of the GraphQL query matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'readFirewallAlias' => [
                    'name' => 'test1',
                    'type' => 'VAL_HOST',
                    'descr' => 'Test alias 1',
                    'address' => ['1.2.3.4'],
                    'detail' => ['test detail 1'],
                ],
            ],
        ]);
    }

    /**
     * Ensure we can query for all Model objects without query_params or pagination.
     */
    public function test_query_all_models_without_params(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();

        $graphql->query->value = '{ queryFirewallAliases { name type } }';
        $graphql->create();
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [
                    ['name' => 'test1', 'type' => 'VAL_HOST'],
                    ['name' => 'test2', 'type' => 'VAL_NETWORK'],
                    ['name' => 'test3', 'type' => 'VAL_PORT'],
                ],
            ],
        ]);
    }

    /**
     * Ensure we can query for a specific Model object using query_params.
     */
    public function test_query_model_with_params(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();
        $graphql->query->value = '{ queryFirewallAliases(query_params: {id: 1}) { name type } }';
        $graphql->create();
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [['name' => 'test2', 'type' => 'VAL_NETWORK']],
            ],
        ]);
    }

    /**
     * Ensure we can limit the number of results returned by using the limit argument.
     */
    public function test_query_models_with_limit(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();

        $graphql->query->value = '{ queryFirewallAliases(limit: 2) { name type } }';
        $graphql->create();
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [
                    ['name' => 'test1', 'type' => 'VAL_HOST'],
                    ['name' => 'test2', 'type' => 'VAL_NETWORK'],
                ],
            ],
        ]);
    }

    /**
     * Ensure we can change the offset of the results returned by using the offset argument.
     */
    public function test_query_models_with_offset(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();

        $graphql->query->value = '{ queryFirewallAliases(offset: 1) { name type } }';
        $graphql->create();
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [
                    ['name' => 'test2', 'type' => 'VAL_NETWORK'],
                    ['name' => 'test3', 'type' => 'VAL_PORT'],
                ],
            ],
        ]);
    }

    /**
     * Ensure we can reverse the order of the results returned by using the reverse argument.
     */
    public function test_query_models_with_reverse(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();

        $graphql->query->value = '{ queryFirewallAliases(reverse: true) { name type } }';
        $graphql->create();
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [
                    ['name' => 'test3', 'type' => 'VAL_PORT'],
                    ['name' => 'test2', 'type' => 'VAL_NETWORK'],
                    ['name' => 'test1', 'type' => 'VAL_HOST'],
                ],
            ],
        ]);
    }

    /**
     * Ensure we can create a new Model object using a GraphQL mutation.
     */
    public function test_create_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value =
            "mutation { createFirewallAlias(name: \"test4\", type: VAL_HOST, descr: \"Test alias 4\", address: [\"4.3.2.1\"], detail: [\"test detail 4\"]) { name type descr address detail } }";
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'createFirewallAlias' => [
                    'name' => 'test4',
                    'type' => 'VAL_HOST',
                    'descr' => 'Test alias 4',
                    'address' => ['4.3.2.1'],
                    'detail' => ['test detail 4'],
                ],
            ],
        ]);

        # Ensure the new object was really created
        $alias_q = FirewallAlias::query(name: 'test4');
        $this->assert_is_true($alias_q->exists());
        $this->assert_equals($alias_q->first()->name->value, 'test4');
        $this->assert_equals($alias_q->first()->type->value, 'host');
        $this->assert_equals($alias_q->first()->descr->value, 'Test alias 4');
        $this->assert_equals($alias_q->first()->address->value, ['4.3.2.1']);
        $this->assert_equals($alias_q->first()->detail->value, ['test detail 4']);

        # Delete the new object
        $alias_q->first()->delete();
    }

    /**
     * Ensure we can update an existing Model object using a GraphQL mutation.
     */
    public function test_update_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value =
            "mutation { updateFirewallAlias(id: 0, type: VAL_NETWORK, descr: \"Test alias 5\", address: [\"5.5.5.5/5\"], detail: [\"test detail 5\"]) { id type descr address detail } }";
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'updateFirewallAlias' => [
                    'id' => 0,
                    'type' => 'VAL_NETWORK',
                    'descr' => 'Test alias 5',
                    'address' => ['5.5.5.5/5'],
                    'detail' => ['test detail 5'],
                ],
            ],
        ]);

        # Ensure the object was really updated
        $alias = new FirewallAlias(id: 0);
        $this->assert_equals($alias->type->value, 'network');
        $this->assert_equals($alias->descr->value, 'Test alias 5');
        $this->assert_equals($alias->address->value, ['5.5.5.5/5']);
        $this->assert_equals($alias->detail->value, ['test detail 5']);
    }

    /**
     * Ensure we can delete an existing Model object using a GraphQL mutation.
     */
    public function test_delete_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value = 'mutation { deleteFirewallAlias(id: 0) { id name } }';
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'deleteFirewallAlias' => [
                    'id' => 0,
                    'name' => 'test1',
                ],
            ],
        ]);

        # Ensure the object was really deleted
        $alias_q = FirewallAlias::query(name: 'test1');
        $this->assert_is_false($alias_q->exists());
    }

    /**
     * Ensure we can replace all Model objects using a GraphQL mutation.
     */
    public function test_replace_all_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value =
            "mutation { replaceAllFirewallAliases(objects: [{name: \"test4\", type: VAL_HOST, descr: \"Test alias 4\", address: [\"4.4.4.4\"], detail: [\"test detail 4\"]}]) { name type descr address detail } }";
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'replaceAllFirewallAliases' => [
                    [
                        'name' => 'test4',
                        'type' => 'VAL_HOST',
                        'descr' => 'Test alias 4',
                        'address' => ['4.4.4.4'],
                        'detail' => ['test detail 4'],
                    ],
                ],
            ],
        ]);

        # Ensure the new object was really created
        $alias_q = FirewallAlias::read_all();
        $this->assert_equals($alias_q->count(), 1);
        $this->assert_equals($alias_q->first()->name->value, 'test4');
        $this->assert_equals($alias_q->first()->type->value, 'host');
        $this->assert_equals($alias_q->first()->descr->value, 'Test alias 4');
        $this->assert_equals($alias_q->first()->address->value, ['4.4.4.4']);
        $this->assert_equals($alias_q->first()->detail->value, ['test detail 4']);
    }

    /**
     * Ensure we can delete all Model objects using a GraphQL mutation.
     */
    public function test_delete_all_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value = 'mutation { deleteAllFirewallAliases { id name } }';
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'deleteAllFirewallAliases' => [
                    ['id' => 0, 'name' => 'test1'],
                    ['id' => 1, 'name' => 'test2'],
                    ['id' => 2, 'name' => 'test3'],
                ],
            ],
        ]);

        # Ensure the objects were really deleted
        $alias_q = FirewallAlias::read_all();
        $this->assert_equals($alias_q->count(), 0);
    }

    /**
     * Ensure we can delete many Model objects via query using a GraphQL mutation
     */
    public function test_delete_many_mutation(): void {
        # Define a new GraphQL model to execute a mutation against.
        $graphql = new GraphQL();
        $graphql->query->value =
            "mutation { deleteManyFirewallAliases(query_params: {name__except: \"test2\"}) { id name } }";
        $graphql->create();

        # Ensure the result of the GraphQL mutation matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'deleteManyFirewallAliases' => [['id' => 0, 'name' => 'test1'], ['id' => 2, 'name' => 'test3']],
            ],
        ]);

        # Ensure the objects were really deleted
        $alias_q = FirewallAlias::read_all();
        $this->assert_equals($alias_q->count(), 1);
        $this->assert_equals($alias_q->first()->name->value, 'test2');
    }

    /**
     * Ensure we can execute a query with variables.
     */
    public function test_query_with_variables(): void {
        # Define a new GraphQL model to execute a query against.
        $graphql = new GraphQL();
        $graphql->query->value =
            "query(\$query_params: QueryParams) { queryFirewallAliases(query_params: \$query_params) { name } }";
        $graphql->variables->value = ['query_params' => ['id__except' => 1]];
        $graphql->create();

        # Ensure the result of the GraphQL query matches the expected values.
        $this->assert_equals($graphql->result->toArray(), [
            'data' => [
                'queryFirewallAliases' => [['name' => 'test1'], ['name' => 'test3']],
            ],
        ]);
    }

    /**
     * Ensure the GraphQL model's internal callable returns an empty array.
     */
    public function test_internal_callable(): void {
        $graphql = new GraphQL();
        $this->assert_equals($graphql->get_internal(), []);
    }

    /**
     * Ensure the GraphQL 'result' property is excluded from serialization. This property is not serializable and will
     * throw an error when the Model tries to set the 'initial_object' property.
     */
    public function test_result_property_excluded_from_serialization(): void {
        $graphql = new GraphQL();
        $this->assert_is_true(!in_array('result', $graphql->__sleep()));
        $this->assert_does_not_throw(
            callable: function () use ($graphql) {
                serialize($graphql);
            },
        );
    }
}
