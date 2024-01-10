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
                subject: (new RESTAPIVersion)->get_api_version()
            ),
            1
        );
    }
    
    /**
     * Checks that the `get_latest_api_version()` correctly returns the latest version available to the system.
     */
    public function test_get_latest_api_version() {
        $this->assert_equals(
            (new RESTAPIVersion)->get_latest_api_version(),
            array_key_first((new RESTAPIVersion)->get_all_available_versions())
        );
    }
}
