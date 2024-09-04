<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\GraphQL\Resolver;
use RESTAPI\Models\FirewallAlias;

class APIGraphQLResolverTestCase extends TestCase
{
    /**
     * Set up FirewallAlias models for testing.
     */
    public function setup(): void
    {
        # First, create several new FirewallAlias models to query.
        $alias = new FirewallAlias();
        $alias->replace_all(
            [
                ['name' => 'test1', 'type' => 'host', 'descr' => 'Test alias 1', 'address' => ['1.2.3.4'], 'detail' => ['test detail 1']],
                ['name' => 'test2', 'type' => 'network', 'descr' => 'Test alias 2', 'address' => ['1.2.3.4/32'], 'detail' => ['test detail 2']],
                ['name' => 'test3', 'type' => 'port', 'descr' => 'Test alias 3', 'address' => ['80', '443'], 'detail' => ['test detail 3']]
            ]
        );
    }

    /**
     * Clean up FirewallAlias models after testing.
     */
    public function teardown(): void
    {
        # Clean up the test data.
        $alias = new FirewallAlias();
        $alias->delete_all();
    }

    /**
     * Ensure the 'query' resolver correctly queries Model objects
     */
    public function test_query_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->query(
            root: null,
            args: [
                'query_params' => ['name' => 'test2'],
                'limit' => 0,
                'offset' => 0,
                'reverse' => false
            ],
            context: null,
            info: null
        );

        # Ensure the query results are as expected
        $this->assert_equals(
            $result[0],
            [
                'id' => 1,
                'name' => 'test2',
                'type' => 'network',
                'descr' => 'Test alias 2',
                'address' => ['1.2.3.4/32'],
                'detail' => ['test detail 2']
            ],
        );
    }

    /**
     * Ensure the 'read' resolver correctly reads Model objects
     */
    public function test_read_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->read(
            root: null,
            args: [
                'id' => 2
            ],
            context: null,
            info: null
        );

        # Ensure the read results are as expected
        $this->assert_equals(
            $result,
            ['id' => 2, 'name' => 'test3', 'type' => 'port', 'descr' => 'Test alias 3', 'address' => ['80', '443'], 'detail' => ['test detail 3']]
        );
    }

    /**
     * Ensure the 'create' resolver correctly creates Model objects
     */
    public function test_create_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->create(
            root: null,
            args: [
                'name' => 'test4',
                'type' => 'host',
                'descr' => 'Test alias 4',
                'address' => ['4.3.2.1'],
                'detail' => ['test detail 4']
            ],
            context: null,
            info: null
        );

        # Ensure the created object is as expected
        $this->assert_equals(
            $result,
            ['id' => 3, 'name' => 'test4', 'type' => 'host', 'descr' => 'Test alias 4', 'address' => ['4.3.2.1'], 'detail' => ['test detail 4']]
        );

        # Ensure the created object is in the config
        $this->assert_is_true($resolver->model->query(name: 'test4')->exists());
    }

    /**
     * Ensure the 'update' resolver correctly updates Model objects
     */
    public function test_update_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->update(
            root: null,
            args: [
                'id' => 1,
                'type' => 'host',
                'descr' => 'Test alias 5',
                'address' => ['1.2.3.4'],
                'detail' => ['test detail 5']
            ],
            context: null,
            info: null
        );

        # Ensure the updated object is as expected
        $this->assert_equals(
            $result,
            ['id' => 1, 'name' => 'test2', 'type' => 'host', 'descr' => 'Test alias 5', 'address' => ['1.2.3.4'], 'detail' => ['test detail 5']]
        );

        # Ensure the updated object is in the config
        $this->assert_is_true($resolver->model->query(descr: 'Test alias 5')->exists());
    }

    /**
     * Ensure the 'delete' resolver correctly deletes Model objects
     */
    public function test_delete_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete(
            root: null,
            args: [
                'id' => 2
            ],
            context: null,
            info: null
        );

        # Ensure the deleted object is as expected
        $this->assert_equals(
            $result,
            ['id' => 2, 'name' => 'test3', 'type' => 'port', 'descr' => 'Test alias 3', 'address' => ['80', '443'], 'detail' => ['test detail 3']]
        );

        # Ensure the deleted object is not in the config
        $this->assert_is_false($resolver->model->query(name: 'test3')->exists());
    }

    /**
     * Ensure the 'replace_all' resolver correctly replaces all Model objects
     */
    public function test_replace_all_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->replace_all(
            root: null,
            args: [
                'objects' => [
                    ['name' => 'test5', 'type' => 'host', 'descr' => 'Test alias 5', 'address' => ['5.5.5.5'], 'detail' => ['test detail 5']],
                    ['name' => 'test6', 'type' => 'network', 'descr' => 'Test alias 6', 'address' => ['6.6.6.6/6'], 'detail' => ['test detail 6']],
                    ['name' => 'test7', 'type' => 'port', 'descr' => 'Test alias 7', 'address' => ['7', '77'], 'detail' => ['test detail 7']],
                ]
            ],
            context: null,
            info: null
        );

        # Ensure the replaced objects are as expected
        $this->assert_equals($resolver->model->read_all()->count(), 3);
        $this->assert_is_true($resolver->model->query(name: 'test5')->exists());
        $this->assert_is_true($resolver->model->query(name: 'test6')->exists());
        $this->assert_is_true($resolver->model->query(name: 'test7')->exists());
    }

    /**
     * Ensure the 'delete_many' resolver correctly deletes multiple Model objects
     */
    public function test_delete_many_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete_many(
            root: null,
            args: [
                'query_params' => ['name' => 'test1'],
                'limit' => 0,
                'offset' => 0,
                'reverse' => false
            ],
            context: null,
            info: null
        );

        # Ensure the deleted objects are as expected
        $this->assert_is_false($resolver->model->query(name: 'test1')->exists());
    }

    /**
     * Ensure the 'delete_all' resolver correctly deletes all Model objects
     */
    public function test_delete_all_resolver(): void
    {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete_all(
            root: null,
            args: [],
            context: null,
            info: null
        );

        # Ensure the deleted objects are as expected
        $this->assert_equals($resolver->model->read_all()->count(), 0);
    }
}