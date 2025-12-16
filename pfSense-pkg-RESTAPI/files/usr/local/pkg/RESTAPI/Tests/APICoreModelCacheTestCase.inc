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
        ModelCache::$cache['MockModelClass'] = new ModelSet();
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
        $mock_model_set = new ModelSet();

        # Cache the mock ModelSet in the ModelCache
        $model_cache::cache_modelset('MockModelClass', $mock_model_set);

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
}
