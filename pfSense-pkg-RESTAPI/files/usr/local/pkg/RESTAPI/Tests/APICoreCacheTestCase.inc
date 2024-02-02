<?php

namespace RESTAPI\Tests;

use RESTAPI\Caches\RESTAPIVersionReleasesCache;
use RESTAPI\Core\Cache;
use RESTAPI\Core\TestCase;

class APICoreCacheTestCase extends TestCase
{
    /**
     * Checks that a ServerError is thrown if the `get_data_to_cache()` method is not overwritten by a child class.
     */
    public function test_get_data_to_cache_must_be_overwritten(): void
    {
        $this->assert_throws_response(
            response_id: 'CACHE_GET_DATA_TO_CACHE_MUST_BE_OVERWRITTEN',
            code: 500,
            callable: function () {
                (new Cache())->process();
            },
        );
    }

    /**
     * Checks that the `get_file_path()` method correctly obtains the file path for the Cache's cache file.
     */
    public function test_get_file_path(): void
    {
        $cache = new Cache();
        $this->assert_equals($cache->get_file_path(), Cache::CACHE_DIR . 'Cache.json');
    }

    /**
     * Checks that the `read()` method correctly reads the cached data from the cache file.
     */
    public function test_read(): void
    {
        # Use the releases cache to test this
        $cache = new RESTAPIVersionReleasesCache();
        $cache->process();
        $this->assert_is_not_empty($cache->read());
    }

    /**
     * Checks that the `process()` method correctly refreshes the cache file with new data.
     */
    public function test_process(): void
    {
        # Use the releases cache to test this
        $cache = new RESTAPIVersionReleasesCache();

        # Remove the cache file and ensure it's empty
        unlink($cache->get_file_path());
        $this->assert_is_false(is_file($cache->get_file_path()));

        # Refresh the cache and ensure there is data again
        $cache->process();
        $this->assert_is_not_empty($cache->read());
    }
}
