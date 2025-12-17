<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\ModelCache;
use RESTAPI\Core\ModelSet;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\DNSResolverHostOverrideAlias;
use RESTAPI\Models\FirewallAlias;
use RESTAPI\Models\RESTAPISettings;

/**
 * Tests for the RESTAPI\Core\ModelCache class. These tests ensure that the central ModelCache object correctly
 * caches and retrieves Model objects as expected.
 */
class APICoreModelCacheTestCase extends TestCase {
    /**
     * Ensures that the ModelCache is always a singleton instance.
     */
    public function test_model_cache_singleton_instance(): void {
        $instance_1 = ModelCache::get_instance();
        $instance_2 = ModelCache::get_instance();

        $this->assert_equals($instance_1, $instance_2);
    }

    /**
     * Ensures the has_modelset() method correctly identifies if a given Model class is cached in the ModelCache.
     */
    public function test_model_cache_has_modelset(): void {
        # Obtain and clear the ModelCache
        $model_cache = ModelCache::get_instance();
        $model_cache::clear();

        # Ensure the ModelCache does not have a cached ModelSet for the MockModelClass
        $this->assert_is_false($model_cache::has_modelset('MockModelClass'));

        # Populate the ModelCache with a mock ModelSet for testing
        ModelCache::$cache['MockModelClass'] = new ModelSet(signature: 'MockModelClass');
        $this->assert_is_true($model_cache::has_modelset('MockModelClass'));

        # Clear the ModelCache and ensure the ModelSet is removed
        $model_cache::clear();
        $this->assert_is_false($model_cache::has_modelset('MockModelClass'));
    }

    /**
     * Ensures the cache_modelset() correctly caches a ModelSet in the ModelCache.
     */
    public function test_model_cache_cache_modelset(): void {
        # Obtain and clear the ModelCache
        $model_cache = ModelCache::get_instance();
        $model_cache::clear();

        # Create a mock ModelSet to cache
        $mock_model_set = new ModelSet(signature: 'MockModelClass');

        # Cache the mock ModelSet in the ModelCache
        $model_cache::cache_modelset($mock_model_set);

        # Ensure the ModelSet was cached correctly
        $this->assert_is_true($model_cache::has_modelset('MockModelClass'));
        $this->assert_equals($model_cache::fetch_modelset('MockModelClass'), $mock_model_set);

        # Clear the ModelCache
        $model_cache::clear();
    }

    /**
     * Ensures the fetch_modelset() method correctly retrieves a cached ModelSet from the ModelCache.
     */
    public function test_model_cache_fetch_nonexisting_modelset_throws_error(): void {
        # Obtain and clear the ModelCache
        $model_cache = ModelCache::get_instance();
        $model_cache::clear();

        # Ensure fetching a non-cached ModelSet throws an error
        $this->assert_throws_response(
            response_id: 'MODEL_CACHE_MODELSET_NOT_FOUND',
            code: 404,
            callable: function () {
                ModelCache::fetch_modelset('MockModelClass');
            },
        );
    }

    /**
     * Ensures a ModelSet without a signature cannot be cached in the ModelCache.
     */
    public function test_model_cache_cache_modelset_without_signature_throws_error(): void
    {
        # Obtain and clear the ModelCache
        $model_cache = ModelCache::get_instance();
        $model_cache::clear();

        # Create a mock ModelSet without a signature
        $mock_model_set = new ModelSet();

        # Ensure caching a ModelSet without a signature throws an error
        $this->assert_throws_response(
            response_id: 'MODEL_CACHE_MODELSET_MISSING_SIGNATURE',
            code: 500,
            callable: function () use ($mock_model_set) {
                ModelCache::cache_modelset($mock_model_set);
            },
        );

        # Clear the ModelCache
        $model_cache::clear();
    }

    /**
     * Ensures Models must be many-enabled to be indexed in the ModelCache.
     */
    public function test_model_index_must_be_many_enabled(): void {
        # Ensure attempting to index a non-many enabled Model throws an error
        $this->assert_throws_response(
            response_id: 'MODEL_CACHE_INDEX_FIELD_ON_NON_MANY_MODEL',
            code: 500,
            callable: function () {
                ModelCache::index_modelset_by_field(model_class: RESTAPISettings::get_class_fqn(), index_field: 'id');
            },
        );
    }

    /**
     * Ensures we cannot index a cached ModelSet by a non-unique field.
     */
    public function test_model_index_field_must_be_unique(): void {
        # Ensure indexing by id is always allowed
        $this->assert_does_not_throw(function () {
            ModelCache::index_modelset_by_field(model_class: FirewallAlias::get_class_fqn(), index_field: 'id');
        });

        # Ensure attempting to index the Model by the non-unique field throws an error
        $this->assert_throws_response(
            response_id: 'MODEL_CACHE_INDEX_FIELD_NOT_UNIQUE',
            code: 500,
            callable: function () {
                ModelCache::index_modelset_by_field(model_class: FirewallAlias::get_class_fqn(), index_field: 'descr');
            },
        );
    }

    /**
     * Ensures we cannot index a cached ModelSet for a Model that has a parent model class.
     */
    public function test_model_index_field_cannot_have_parent_model_class(): void {
        # Ensure attempting to index the Model by the non-unique field throws an error
        $this->assert_throws_response(
            response_id: 'MODEL_CACHE_INDEX_FIELD_ON_PARENTED_MODEL',
            code: 500,
            callable: function () {
                ModelCache::index_modelset_by_field(
                    model_class: DNSResolverHostOverrideAlias::get_class_fqn(),
                    index_field: 'id',
                );
            },
        );
    }

    /**
     * Ensures the index_modelset_by_field() method correctly indexes a cached ModelSet by the specified field. This
     * test also ensures the has_model() and fetch_model() methods work as expected for indexed Model objects.
     */
    public function test_model_index_modelset_by_field(): void {
        # Create FirewallAlias model objects to test with
        $alias1 = new FirewallAlias(data: ['name' => 'host_alias', 'descr' => 'First alias', 'type' => 'host']);
        $alias2 = new FirewallAlias(data: ['name' => 'network_alias', 'descr' => 'Second alias', 'type' => 'network']);
        $alias3 = new FirewallAlias(data: ['name' => 'port_alias', 'descr' => 'Third alias', 'type' => 'port']);
        $alias1->create();
        $alias2->create();
        $alias3->create();

        # Index the FirewallAlias ModelSet by the 'name' field
        ModelCache::index_modelset_by_field(model_class: FirewallAlias::get_class_fqn(), index_field: 'name');

        # Ensure the ModelCache has the indexed Models
        $this->assert_is_true(
            ModelCache::has_model(
                model_class: FirewallAlias::get_class_fqn(),
                index_field: 'name',
                index_value: 'host_alias',
            ),
        );
        $this->assert_is_true(
            ModelCache::has_model(
                model_class: FirewallAlias::get_class_fqn(),
                index_field: 'name',
                index_value: 'network_alias',
            ),
        );
        $this->assert_is_true(
            ModelCache::has_model(
                model_class: FirewallAlias::get_class_fqn(),
                index_field: 'name',
                index_value: 'port_alias',
            ),
        );

        # Fetch the indexed Models and ensure they are correct
        $fetched_alias1 = ModelCache::fetch_model(
            model_class: FirewallAlias::get_class_fqn(),
            index_field: 'name',
            index_value: 'host_alias',
        );
        $this->assert_equals($fetched_alias1->name->value, 'host_alias');
        $fetched_alias2 = ModelCache::fetch_model(
            model_class: FirewallAlias::get_class_fqn(),
            index_field: 'name',
            index_value: 'network_alias',
        );
        $this->assert_equals($fetched_alias2->name->value, 'network_alias');
        $fetched_alias3 = ModelCache::fetch_model(
            model_class: FirewallAlias::get_class_fqn(),
            index_field: 'name',
            index_value: 'port_alias',
        );
        $this->assert_equals($fetched_alias3->name->value, 'port_alias');

        # Clean up the created aliases
        FirewallAlias::delete_all();
    }
}
