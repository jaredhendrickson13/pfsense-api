<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Auth;
use RESTAPI\Core\TestCase;
use RESTAPI\GraphQL\Resolver;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\User;

class APIGraphQLResolverTestCase extends TestCase {
    /**
     * @var Auth $auth;
     * Auth object to pass to GraphQL for privilege checks.
     */
    public Auth $auth;

    /**
     * Set up FirewallAlias models for testing.
     */
    public function setup(): void {
        # Ensure we have a valid Auth object to pass to GraphQL for privilege checks.
        $this->auth = new Auth();
        $this->auth->username = 'admin';

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
     * Ensure the 'query' resolver correctly queries Model objects
     */
    public function test_query_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->query(
            root: null,
            args: [
                'query_params' => ['name' => 'test2'],
                'limit' => 0,
                'offset' => 0,
                'reverse' => false,
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the query results are as expected
        $this->assert_equals($result[0], [
            'id' => 1,
            'name' => 'test2',
            'type' => 'network',
            'descr' => 'Test alias 2',
            'address' => ['1.2.3.4/32'],
            'detail' => ['test detail 2'],
        ]);
    }

    /**
     * Ensure the 'read' resolver correctly reads Model objects
     */
    public function test_read_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->read(
            root: null,
            args: [
                'id' => 2,
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the read results are as expected
        $this->assert_equals($result, [
            'id' => 2,
            'name' => 'test3',
            'type' => 'port',
            'descr' => 'Test alias 3',
            'address' => ['80', '443'],
            'detail' => ['test detail 3'],
        ]);
    }

    /**
     * Ensure the 'create' resolver correctly creates Model objects
     */
    public function test_create_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->create(
            root: null,
            args: [
                'name' => 'test4',
                'type' => 'host',
                'descr' => 'Test alias 4',
                'address' => ['4.3.2.1'],
                'detail' => ['test detail 4'],
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the created object is as expected
        $this->assert_equals($result, [
            'id' => 3,
            'name' => 'test4',
            'type' => 'host',
            'descr' => 'Test alias 4',
            'address' => ['4.3.2.1'],
            'detail' => ['test detail 4'],
        ]);

        # Ensure the created object is in the config
        $this->assert_is_true($resolver->model->query(name: 'test4')->exists());
    }

    /**
     * Ensure the 'update' resolver correctly updates Model objects
     */
    public function test_update_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->update(
            root: null,
            args: [
                'id' => 1,
                'type' => 'host',
                'descr' => 'Test alias 5',
                'address' => ['1.2.3.4'],
                'detail' => ['test detail 5'],
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the updated object is as expected
        $this->assert_equals($result, [
            'id' => 1,
            'name' => 'test2',
            'type' => 'host',
            'descr' => 'Test alias 5',
            'address' => ['1.2.3.4'],
            'detail' => ['test detail 5'],
        ]);

        # Ensure the updated object is in the config
        $this->assert_is_true($resolver->model->query(descr: 'Test alias 5')->exists());
    }

    /**
     * Ensure the 'delete' resolver correctly deletes Model objects
     */
    public function test_delete_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete(
            root: null,
            args: [
                'id' => 2,
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the deleted object is as expected
        $this->assert_equals($result, [
            'id' => 2,
            'name' => 'test3',
            'type' => 'port',
            'descr' => 'Test alias 3',
            'address' => ['80', '443'],
            'detail' => ['test detail 3'],
        ]);

        # Ensure the deleted object is not in the config
        $this->assert_is_false($resolver->model->query(name: 'test3')->exists());
    }

    /**
     * Ensure the 'replace_all' resolver correctly replaces all Model objects
     */
    public function test_replace_all_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->replace_all(
            root: null,
            args: [
                'objects' => [
                    [
                        'name' => 'test5',
                        'type' => 'host',
                        'descr' => 'Test alias 5',
                        'address' => ['5.5.5.5'],
                        'detail' => ['test detail 5'],
                    ],
                    [
                        'name' => 'test6',
                        'type' => 'network',
                        'descr' => 'Test alias 6',
                        'address' => ['6.6.6.6/6'],
                        'detail' => ['test detail 6'],
                    ],
                    [
                        'name' => 'test7',
                        'type' => 'port',
                        'descr' => 'Test alias 7',
                        'address' => ['7', '77'],
                        'detail' => ['test detail 7'],
                    ],
                ],
            ],
            context: ['auth' => $this->auth],
            info: null,
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
    public function test_delete_many_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete_many(
            root: null,
            args: [
                'query_params' => ['name' => 'test1'],
                'limit' => 0,
                'offset' => 0,
                'reverse' => false,
            ],
            context: ['auth' => $this->auth],
            info: null,
        );

        # Ensure the deleted objects are as expected
        $this->assert_is_false($resolver->model->query(name: 'test1')->exists());
    }

    /**
     * Ensure the 'delete_all' resolver correctly deletes all Model objects
     */
    public function test_delete_all_resolver(): void {
        $resolver = new Resolver(model: new FirewallAlias());
        $result = $resolver->delete_all(root: null, args: [], context: ['auth' => $this->auth], info: null);

        # Ensure the deleted objects are as expected
        $this->assert_equals($resolver->model->read_all()->count(), 0);
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the query resolver
     */
    public function test_check_query_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_query_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot query the model without an allowed privilege when running the query method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->query(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the query resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'query', auth: $auth);
            },
        );

        # Use the admin user to ensure we can query the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the query resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'query', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the read resolver
     */
    public function test_check_read_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_read_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot read the model without an allowed privilege when running the read method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->read(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the read resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'read', auth: $auth);
            },
        );

        # Use the admin user to ensure we can read the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the read resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'read', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the create resolver
     */
    public function test_check_create_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_create_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot create the model without an allowed privilege when running the create method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->create(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the create resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'create', auth: $auth);
            },
        );

        # Use the admin user to ensure we can create the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the create resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'create', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the update resolver
     */
    public function test_check_update_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_update_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot update the model without an allowed privilege when running the update method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->update(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the update resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'update', auth: $auth);
            },
        );

        # Use the admin user to ensure we can update the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the update resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'update', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the delete resolver
     */
    public function test_check_delete_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_delete_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot delete the model without an allowed privilege when running the delete method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->delete(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the delete resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete', auth: $auth);
            },
        );

        # Use the admin user to ensure we can delete the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the delete resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the replace_all resolver
     */
    public function test_check_replace_all_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_replace_all_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot replace_all the model without an allowed privilege when running the replace_all method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->replace_all(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the replace_all resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'replace_all', auth: $auth);
            },
        );

        # Use the admin user to ensure we can replace_all the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the replace_all resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'replace_all', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the delete_many resolver
     */
    public function test_check_delete_many_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_delete_many_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot delete_many the model without an allowed privilege when running the delete_many method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->delete_many(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the delete_many resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete_many', auth: $auth);
            },
        );

        # Use the admin user to ensure we can delete_many the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the delete_many resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete_many', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }

    /**
     * Ensure the 'check_privs' method correctly checks privileges for the delete_all resolver
     */
    public function test_check_delete_all_privilege(): void {
        $resolver = new Resolver(model: new FirewallAlias());

        # Create a user we can use for testing
        $user = new User(name: 'test_check_delete_all_privilege', password: 'testpass', priv: []);
        $user->create();
        $auth = new Auth();
        $auth->username = $user->name->value;

        # Ensure we cannot delete_all the model without an allowed privilege when running the delete_all method
        $this->assert_throws(
            exceptions: ['GraphQL\Error\Error'],
            callable: function () use ($resolver, $user, $auth) {
                $resolver->delete_all(root: null, args: [], context: ['auth' => $auth], info: null);
            },
        );

        # Ensure we cannot call check_privs for the delete_all resolver without an allowed privilege
        $this->assert_throws_response(
            response_id: 'GRAPHQL_RESOLVER_UNAUTHORIZED',
            code: 403,
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete_all', auth: $auth);
            },
        );

        # Use the admin user to ensure we can delete_all the model with an allowed privilege
        $auth->username = 'admin';

        # Ensure we can call check_privs for the delete_all resolver with an allowed privilege
        $this->assert_does_not_throw(
            callable: function () use ($resolver, $user, $auth) {
                $resolver->check_privs(resolver: 'delete_all', auth: $auth);
            },
        );

        # Delete the user
        $user->delete();
    }
}
