<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\RESTAPIVersion;

class APIModelsAPIVersionTestCase extends TestCase
{
    /**
     * Checks that we are able to accurately obtain the current API version
     */
    public function test_get_api_version() {
        # Ensure the version identified by this method is a semver
        $this->assert_equals(
            preg_match(
                pattern: "/^v(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$/",
                subject: RESTAPIVersion::get_api_version()
            ),
            1
        );
    }
    
    /**
     * Checks that the `get_latest_api_version()` correctly returns the latest version available to the system.
     */
    public function test_get_latest_api_version() {
        $this->assert_equals(
            RESTAPIVersion::get_latest_api_version(),
            array_key_first(RESTAPIVersion::get_all_available_versions())
        );
    }
    
    /**
     * Checks that the releases cache file is created after running `get_github_releases()`
     */
    public function test_releases_are_cached() {
        # Remove any existing cache file
        unlink(RESTAPIVersion::RELEASES_CACHE_FILE);
        
        # Ensure the releases are cached after running `get_github_releases()`
        $releases = RESTAPIVersion::get_github_releases();
        $releases_cache = json_decode(file_get_contents(RESTAPIVersion::RELEASES_CACHE_FILE), associative: true);
        $this->assert_equals($releases["releases"], $releases_cache["releases"]);
    }

}
