<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\AvailablePackage;
use RESTAPI\Models\Package;

class APIModelsAvailablePackageTestCase extends TestCase {
    /**
     * Checks that available packages can be successfully read from the external package repos.
     */
    public function test_read_available_packages(): void {
        # Try to obtain info about the pfSense-pkg-sudo package from remote repos
        $avail_pkgs = AvailablePackage::query(name: 'pfSense-pkg-sudo')->first();

        # Ensure details about this package are accurate
        $this->assert_equals($avail_pkgs->name->value, 'pfSense-pkg-sudo');
        $this->assert_equals($avail_pkgs->shortname->value, 'sudo');
        $this->assert_is_not_empty($avail_pkgs->descr->value);
        $this->assert_is_not_empty($avail_pkgs->version->value);
        $this->assert_str_contains($avail_pkgs->deps->value[0], 'sudo-');

        # Install the package and ensure the `installed` field is now `true`
        $sudo_pkg = new Package(data: ['name' => 'pfSense-pkg-sudo']);
        $sudo_pkg->create();
        $avail_pkgs->reload_config();
        $avail_pkgs->from_internal();
        $this->assert_is_true($avail_pkgs->installed->value);

        # Delete the package and ensure the `installed` field is once again `false`
        $sudo_pkg->delete();
        $avail_pkgs->reload_config();
        $avail_pkgs->from_internal();
        $this->assert_is_false($avail_pkgs->installed->value);
    }
}
