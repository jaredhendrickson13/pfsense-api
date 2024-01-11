<?php

namespace RESTAPI\Tests;

use RESTAPI\Caches\AvailablePackageCache;
use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;
use RESTAPI\Models\Package;

class APIModelsPackageTestCase extends TestCase
{
    /**
     * Checks that packages are installed correctly when the Package model's `create()` method runs and is correctly
     * deleted when the Package model's `delete()` method is run.
     */
    public function test_package_install_and_delete(): void {
        # Try to install the pfSense-pkg-sudo package using the Package model
        $pkg = new Package(data: ["name" => "pfSense-pkg-sudo"]);
        $pkg->create();

        # Ensure the $pkg now contains information about the installed package
        $this->assert_equals($pkg->shortname->value, "sudo");
        $this->assert_is_greater_than_or_equal($pkg->id, 0);
        $this->assert_is_not_empty($pkg->descr->value);
        $this->assert_is_not_empty($pkg->installed_version->value);
        $this->assert_is_not_empty($pkg->latest_version->value);

        # Ensure the package is really installed via pkg
        $pkg_info = new Command("pkg-static info pfSense-pkg-sudo");
        $this->assert_equals($pkg_info->result_code, 0);
        $this->assert_str_contains($pkg_info->output, "Installed on");

        # Ensure we can now read the installed package using the Package model
        $this->assert_is_true(Package::query(name: "pfSense-pkg-sudo")->exists());

        # Delete the package and ensure it is no longer present on the system
        $pkg->delete();
        $pkg_info = new Command("pkg-static info pfSense-pkg-sudo");
        $this->assert_equals($pkg_info->result_code, 1);
        $this->assert_str_contains($pkg_info->output, "pkg-static: No package(s) matching pfSense-pkg-sudo");

        # Ensure we can no longer read the installed package using the Package model
        $this->assert_is_false(Package::query(name: "pfSense-pkg-sudo")->exists());
    }

    /**
     * Checks that the `latest_version` field's value is correctly derived from the available packages cache and the
     * `update_available` field correctly displays when an update is actually available.
     */
    public function test_version_discovery(): void {
        # Install a package and ensure the no update is advertised
        $pkg = new Package(data: ["name" => "pfSense-pkg-sudo"]);
        $pkg->create();
        $this->assert_is_false($pkg->update_available->value);
        $this->assert_equals($pkg->installed_version->value, $pkg->latest_version->value);

        # Adjust the available packages cache so it thinks an update is available
        $pkgs_cache = new AvailablePackageCache();
        $pkgs_cache_data = $pkgs_cache->read();
        $pkgs_cache_copy = $pkgs_cache_data;
        foreach ($pkgs_cache_data as $pkg_id => $pkg_info) {
            if ($pkg_info["name"] === $pkg->name->value) {
                $pkgs_cache_data[$pkg_id]["version"] = "100.0_0";
                break;
            }
        }
        file_put_contents($pkgs_cache->get_file_path(), json_encode($pkgs_cache_data));

        # Refresh the Package object and ensure the package now shows an update is available
        $pkg->from_internal();
        $this->assert_is_true($pkg->update_available->value);
        $this->assert_equals($pkg->latest_version->value, "100.0_0");

        # Restore the packages cache to its original state and delete the package
        file_put_contents($pkgs_cache->get_file_path(), json_encode($pkgs_cache_copy));
        $pkg->delete();
    }
}
