<?php

namespace API\Tests;

use API\Core\TestCase;
use API\Models\APIVersion;

class APIModelsAPIVersionTestCase extends TestCase
{
    /**
     * Checks that we are able to accurately obtain the current API version
     */
    public function test_get_api_version() {
        # Ensure the version identified by this method is a semver
        echo APIVersion::get_api_version();
        $this->assert_is_true(
            preg_match(
                pattern: "^v(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$",
                subject: APIVersion::get_api_version()
            )
        );
    }

}
