<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\SystemVersion;

class APIModelsSystemVersionTestCase extends TestCase
{
    /**
     * Checks that the `get_pfsense_version()` method correctly obtains the pfSense version details by the version
     * files installed on the system.
     */
    public function test_read()
    {
        # Create a SystemVersion object and ensure its values match the version files on this system
        $system_version = new SystemVersion();

        # Ensure the `version` field contains the full release name
        $this->assert_equals(
            $system_version->version->value . PHP_EOL,
            file_get_contents($system_version::VERSION_PATH),
        );

        # Ensure the `patch` field contains the current patch version
        $this->assert_equals(
            $system_version->patch->value . PHP_EOL,
            file_get_contents($system_version::VERSION_PATCH_PATH),
        );

        # Ensure the `buildtime` field contains the full release name
        $this->assert_equals(
            $system_version->buildtime->value . PHP_EOL,
            file_get_contents($system_version::VERSION_BUILDTIME_PATH),
        );

        # Ensure the `base` version contains only the major and minor versions
        $version = explode('-', $system_version->version->value)[0];
        $this->assert_equals($system_version->base->value, $version);
    }
}
