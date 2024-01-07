<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\AvailablePackage;

class APIModelsAvailablePackageTestCase extends TestCase
{
    /**
     * Checks that available packages can be successfully read from the external package repos.
     */
    public function test_read_available_packages(): void {
        # Try to obtain info about the pfSense-pkg-sudo package from remote repos
        $pkg = AvailablePackage::query(name: "pfSense-pkg-sudo")->first();

        # Ensure details about this package are accurate
        $this->assert_equals($pkg->name->value, "pfSense-pkg-sudo");
        $this->assert_equals($pkg->shortname->value, "sudo");
        $this->assert_is_not_empty($pkg->descr->value);
        $this->assert_is_not_empty($pkg->version->value);
        $this->assert_str_contains($pkg->deps->value[0], "sudo-");
        $this->assert_is_false($pkg->installed->value);

        # Install the package and ensure the `installed` field is now `true`
        # TODO: change this to use the Package model to install the package
        new Command("pkg-static install -y pfSense-pkg-sudo");
        $pkg->from_internal();
        $this->assert_is_true($pkg->installed->value);

        # Delete the package and ensure the `installed` field is once again `false`
        # TODO: change this to use the Package model to delete the package
        new Command("pkg-static delete -y pfSense-pkg-sudo");
        $pkg->from_internal();
        $this->assert_is_false($pkg->installed->value);
    }
}