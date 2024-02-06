#!/usr/local/bin/php -f
<?php
//   Copyright 2024 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Caches\PrivilegesCache;
use RESTAPI\Core\Cache;
use RESTAPI\Dispatchers\WebGUIRestartDispatcher;
use RESTAPI\Models\RESTAPIJWT;
use RESTAPI\Models\RESTAPISettings;
use RESTAPI\Models\RESTAPISettingsSync;
use function RESTAPI\Core\Tools\cprint;
use function RESTAPI\Core\Tools\get_classes_from_namespace;

/**
 * Builds a PHP API endpoint in the pfSense webroot for each Endpoint class defined in \RESTAPI\Endpoints using the
 * Endpoint class's specified $url property.
 */
function build_endpoints(): void {
    # Print that we are starting to build endpoints
    print 'Building endpoints... ';

    # Import each endpoint class
    foreach (glob('/usr/local/pkg/RESTAPI/Endpoints/*.inc') as $file) {
        # Import classes files and create object
        require_once $file;
        $endpoint_class = '\\RESTAPI\\Endpoints\\' . str_replace('.inc', '', basename($file));
        $endpoint_obj = new $endpoint_class();

        # Exit if the endpoint fails to be built
        if (!$endpoint_obj->build_endpoint_url()) {
            print "failed! ($endpoint_obj->url)";
            exit(1);
        }
    }

    # Print that the build is done if we made it through the loop
    print 'done.' . PHP_EOL;

    # Rebuild privileges in case any Endpoint privilege names have changed
    build_privs();
}

/**
 * Builds a PHP webConfigurator page in the pfSense webroot for each Form class defined in \RESTAPI\Forms using the
 * Form class's specified $url property.
 */
function build_forms(): void {
    # Print that we are starting to build forms
    print 'Building forms... ';

    # Import each form class
    foreach (glob('/usr/local/pkg/RESTAPI/Forms/*.inc') as $file) {
        # Import classes files and create object
        require_once $file;
        $form_class = '\\RESTAPI\\Forms\\' . str_replace('.inc', '', basename($file));
        $form_obj = new $form_class();

        # Exit if the form fails to be built
        if (!$form_obj->build_form_url()) {
            print "failed! ($form_obj->url)";
            exit(1);
        }
    }

    # Print that the build is done if we made it through the loop
    print 'done.' . PHP_EOL;

    # Rebuild privileges in case any Form privilege names have changed
    build_privs();
}

/**
 * Automatically creates pfSense privileges for each Endpoint class defined in \RESTAPI\Endpoints and each Form class
 * defined in \RESTAPI\Forms.
 */
function build_privs(): void {
    echo 'Building privileges... ';

    # Use PrivilegesCache to refresh the privileges cache file
    $privileges_cache = new PrivilegesCache();
    $privileges_cache->process();

    echo 'done.' . PHP_EOL;
}

/**
 * Runs the process for a specified Dispatcher class in \RESTAPI\Dispatchers.
 * @param string|null $dispatcher_name
 * @note This function does not call the Dispatcher process asynchronously, it will wait for the process to complete.
 */
function notify_dispatcher(string|null $dispatcher_name): void {
    # Format the fully qualified class name
    $class = "\\RESTAPI\\Dispatchers\\$dispatcher_name";

    # Ensure this Dispatcher class exists
    if (class_exists($class)) {
        # Start the Dispatcher process
        echo "Running $class process... ";
        $dispatcher = new $class();
        $dispatcher->process();
        echo 'done.' . PHP_EOL;
        exit(0);
    } else {
        echo "No dispatcher exists at $class" . PHP_EOL;
        exit(1);
    }
}

/**
 * Creates cron jobs for all Dispatcher classes in \RESTAPI\Dispatchers and all Cache classes in \RESTAPI\Caches
 * that have configured schedules.
 */
function schedule_dispatchers(): void {
    # Variables
    $dispatchers = get_classes_from_namespace('\\RESTAPI\\Dispatchers\\');
    $caches = get_classes_from_namespace('\\RESTAPI\\Caches\\');

    # Include both Dispatcher classes and Cache classes. Cache classes inherit from the Dispatcher class.
    $classes = array_merge($dispatchers, $caches);

    # Loop through each defined Dispatcher class and setup the cron jobs for dispatchers with schedules
    foreach ($classes as $class) {
        $dispatcher = new $class();
        if ($dispatcher->schedule) {
            # Start setting up the schedules
            echo "Configuring schedule for $class... ";
            $dispatcher->setup_schedule();

            # For Cache objects, run the Cache process to populate the cache file initially
            if ($dispatcher instanceof Cache) {
                $dispatcher->process();
            }

            # Print that we are done setting up the schedule for this Dispatcher
            echo 'done.' . PHP_EOL;
        }
    }
}

/**
 * Refreshes the cache file by obtaining new day for a given Cache object.
 * @param string|null $cache_name The shortname of the Cache class that should have its cache file refreshed.
 */
function refresh_cache(string|null $cache_name): void {
    # Format the fully qualified class name
    $class = "\\RESTAPI\\Caches\\$cache_name";

    # Ensure this Dispatcher class exists
    if (class_exists($class)) {
        # Start the Dispatcher process
        echo "Refreshing $class cache file... ";
        $cache = new $class();
        $cache->process();
        echo 'done.' . PHP_EOL;
        exit(0);
    } else {
        echo "No cache exists at $class" . PHP_EOL;
        exit(1);
    }
}

/**
 * Runs all (or select) TestCase classes in \RESTAPI\Tests. This is only intended to test development of this package
 * and should not be used on live installs.
 * @param $contains string|null Only run tests that contain this sub-string in the test name.
 * @note Tests will attempt to create, modify and delete configurations and files as well as restart services; which
 * can be disruptive to live systems.
 */
function run_tests(string|null $contains = ''): void {
    # Variables
    $test_cases = glob('/usr/local/pkg/RESTAPI/Tests/*.inc');
    $exit_code = 0;
    $test_count = count($test_cases);
    $skipped_count = 0;
    $succeed_count = 0;
    $failed_tests = [];

    # Print that we are starting tests now
    echo 'Running tests...';

    # Import each test class and run the test
    foreach ($test_cases as $test_case_file) {
        # Import classes files and create object
        require_once $test_case_file;
        $test_case = '\\RESTAPI\\Tests\\' . str_replace('.inc', '', basename($test_case_file));

        # Only run this test case if the test name contains the $contains string
        if (!str_contains($test_case, $contains)) {
            $skipped_count++;
            continue;
        }

        # Try to run the test case.
        $test_obj = new $test_case();
        try {
            $test_obj->run();
            $succeed_count++;
            echo '.'; // Print a dot for each test completed so the user is aware that tests are really running
            $fail_results = null;
        } catch (AssertionError $fail_results) {
            # If an AssertionError is received, there was a test failure. Print the traceback.
            echo 'F';
            $exit_code = 2;
        } catch (Exception | Error $fail_results) {
            echo 'E';
            $exit_code = 1;
        }

        if ($fail_results) {
            $exc_class = $fail_results::class;
            $fail_msg = $fail_results->getMessage();
            $result = '---------------------------------------------------------' . PHP_EOL;
            $result .= "Failure message: [$exc_class] $fail_msg" . PHP_EOL;
            $result .= "Test name: $test_obj->method" . PHP_EOL;
            $result .= "Test description: $test_obj->method_docstring" . PHP_EOL . PHP_EOL;
            $result .= $fail_results->getTraceAsString() . PHP_EOL;
            $failed_tests[] = $result;
        }
    }

    # Mark the test as a failure if there are any failed tests, otherwise mark tests as done.
    echo $failed_tests ? ' failed!' . PHP_EOL : ' done.' . PHP_EOL;

    # Loop through each failed test and print its results
    foreach ($failed_tests as $failed_test) {
        echo $failed_test;
    }

    # Adjust the total test count if Tests were skipped.
    if ($skipped_count > 0) {
        $test_count = $test_count - $skipped_count;
    }

    echo '---------------------------------------------------------' . PHP_EOL;
    echo "Ran all Tests: $succeed_count/$test_count Tests passed. $skipped_count Tests skipped." . PHP_EOL;
    exit($exit_code);
}

/**
 * Restarts the webConfigurator in the background.
 * @note When this package is first installed, this function runs to automatically reload the webConfigurator and
 * apply the nginx changes required for this package to operate. Thus eliminating the requirement for the user
 * to run /etc/rc.restart_webgui after installation.
 */
function restart_webgui(): void {
    echo 'Initiating webConfigurator restart... ';
    (new WebGUIRestartDispatcher())->spawn_process();
    echo 'done.' . PHP_EOL;
    exit(0);
}

/**
 * Creates a backup of the REST API configuration if `keep_backup` is enabled. The backup will be stored in
 * /usr/local/share/pfSense-pkg-RESTAPI/backup.json
 */
function backup(): void {
    echo 'Backing up REST API configuration... ';
    echo match (RESTAPISettings::backup_to_file()) {
        RESTAPI\Models\API_SETTINGS_BACKUP_SUCCESS => 'done.' . PHP_EOL,
        RESTAPI\Models\API_SETTINGS_BACKUP_NOT_CONFIGURED => 'not configured.' . PHP_EOL,
        default => 'unknown error occurred.' . PHP_EOL,
    };
}

/**
 * Restores the latest REST API configuration backup from /usr/local/share/pfSense-pkg-RESTAPI/backup.json
 */
function restore(): void {
    echo 'Restoring REST API configuration... ';
    echo match (RESTAPISettings::restore_from_backup()) {
        RESTAPI\Models\API_SETTINGS_RESTORE_SUCCESS => 'done.' . PHP_EOL,
        RESTAPI\Models\API_SETTINGS_BACKUP_NOT_CONFIGURED => 'not configured.' . PHP_EOL,
        RESTAPI\Models\API_SETTINGS_RESTORE_NO_CHANGE => 'no changes found.' . PHP_EOL,
        RESTAPI\Models\API_SETTINGS_RESTORE_NO_BACKUP => 'no backup found.' . PHP_EOL,
        default => 'unknown error occurred.' . PHP_EOL,
    };
}

/**
 * Syncs the REST API configuration to HA peers if enabled.
 */
function sync(): void {
    RESTAPISettingsSync::sync(print_status: true);
}

/**
 * Updates this package to the latest version available to this system
 */
function update(): void {
    $pf_version = RESTAPI\Core\Tools\get_pfsense_version()['base'];
    echo shell_exec('/usr/local/sbin/pkg-static delete -y pfSense-pkg-RESTAPI');
    echo shell_exec(
        '/usr/local/sbin/pkg-static -C /dev/null add https://github.com/jaredhendrickson13/pfsense-RESTAPI/releases/latest/download/pfSense-' .
            $pf_version .
            '-pkg-REST API.pkg',
    );
    echo shell_exec('/etc/rc.restart_webgui');
}

/**
 * Reverts or updates the REST API package to a specific version.
 * @param $version string semantic version tag to revert or upgrade to.
 */
function revert(string $version): void {
    # Local variables
    $pf_version = RESTAPI\Core\Tools\get_pfsense_version()['base'];
    $url =
        'https://github.com/jaredhendrickson13/pfsense-RESTAPI/releases/download/' .
        urlencode($version) .
        '/pfSense-' .
        $pf_version .
        '-pkg-REST API.pkg';
    echo 'Locating pfSense-pkg-RESTAPI-' . $version . ' at ' . $url . '... ';

    # Check our URL for the existence of this version
    $headers = @get_headers($url);

    # If this release exists, remove the existing package and install the requested one. Otherwise return error
    if ($headers && strpos($headers[0], '302')) {
        echo 'done.' . PHP_EOL;
        echo shell_exec('/usr/local/sbin/pkg-static delete -y pfSense-pkg-RESTAPI');
        echo shell_exec('/usr/local/sbin/pkg-static -C /dev/null add ' . escapeshellarg($url));
        echo shell_exec('/etc/rc.restart_webgui');
    } else {
        echo 'no package found.' . PHP_EOL;
        exit(1);
    }
}

/**
 * Delete the REST API package and restart the webConfigurator to remove nginx changes.
 */
function delete() {
    echo shell_exec('/usr/local/sbin/pkg-static delete -y pfSense-pkg-RESTAPI');
    echo shell_exec('/etc/rc.restart_webgui');
}

/**
 * Rotates the JWT server key. Warning: This will revoke any active JWTs.
 */
function rotate_server_key(): void {
    $pkg_index = RESTAPISettings::get_pkg_id();
    config_set_path("installedpackages/package/$pkg_index/conf/keys", []);
    echo 'Rotating REST API server key... ';
    RESTAPIJWT::init_server_key(rotate: true);
    echo 'done.' . PHP_EOL;
    sync();
}

/**
 * Prints the pfSense-pkg-RESTAPI version information.
 */
function version(): void {
    echo shell_exec('/usr/local/sbin/pkg-static info pfSense-pkg-RESTAPI');
}

/**
 * Prints the pfsense-restapi help page.
 */
function help(): void {
    echo 'pfsense-restapi - CLI tool for pfSense REST API management' . PHP_EOL;
    echo 'Copyright - ' . date('Y') . '© - Jared Hendrickson' . PHP_EOL;
    echo 'SYNTAX:' . PHP_EOL;
    echo '  pfsense-restapi <command> <args>' . PHP_EOL;
    echo 'COMMANDS:' . PHP_EOL;
    echo '  version             : Display the current package version and build information' . PHP_EOL;
    echo '  help                : Display the help page (this page)' . PHP_EOL;
    echo '  buildendpoints      : Build all REST API Endpoints included in this package' . PHP_EOL;
    echo '  buildforms          : Build all REST API Forms included in this package' . PHP_EOL;
    echo '  buildprivs          : Build all REST API privileges included in this package' . PHP_EOL;
    echo '  generatedocs        : Regenerates the OpenAPI documentation' . PHP_EOL;
    echo '  notifydispatcher    : Start a dispatcher process' . PHP_EOL;
    echo '  scheduledispatchers : Sets up cron jobs for dispatchers and caches on a schedule.' . PHP_EOL;
    echo '  refreshcache        : Refresh the cache file for a given cache class.' . PHP_EOL;
    echo '  runtests            : Run all REST API unit Tests. Warning: this may be disruptive!' . PHP_EOL;
    echo '  restartwebgui       : Restart the webConfigurator in the background' . PHP_EOL;
    echo '  update              : Update package to the latest stable version available' . PHP_EOL;
    echo '  revert              : Revert package to a specified version' . PHP_EOL;
    echo '  delete              : Delete package from this system' . PHP_EOL;
    echo '  rotateserverkey     : Rotate the REST API server key and remove all existing tokens' . PHP_EOL;
    echo '  backup              : Create a backup of the REST API configuration' . PHP_EOL;
    echo '  restore             : Restore the REST API configuration from the latest backup' . PHP_EOL;
    echo "  sync                : Sync this system's REST API configuration to configured HA nodes" . PHP_EOL;
    echo PHP_EOL;
}

# BUILD_FORMS COMMAND
if ($argv[1] == 'buildforms') {
    build_forms();
}
# BUILDENDPOINTS COMMAND
elseif ($argv[1] == 'buildendpoints') {
    build_endpoints();
}
# BUILDPRIVS COMMAND
elseif ($argv[1] == 'buildprivs') {
    build_privs();
}
# GENERATEDOCUMENTATION COMMAND
elseif ($argv[1] == 'generatedocs') {
    echo 'Generating OpenAPI documentation... ';
    RESTAPI\Core\Tools\generate_documentation();
    echo 'done.' . PHP_EOL;
}
# NOTIFY_DISPATCHER COMMAND
elseif ($argv[1] == 'notifydispatcher') {
    notify_dispatcher(dispatcher_name: $argv[2]);
}
# SCHEDULE_DISPATCHER COMMAND
elseif ($argv[1] == 'scheduledispatchers') {
    schedule_dispatchers();
}
# REFRESH_CACHE COMMAND
elseif ($argv[1] == 'refreshcache') {
    refresh_cache(cache_name: $argv[2]);
}
# RUNTESTS COMMAND
elseif ($argv[1] == 'runtests') {
    run_tests(contains: $argv[2]);
}
# RESTART_WEBGUI COMMAND
elseif ($argv[1] == 'restartwebgui') {
    restart_webgui();
}
# BACKUP COMMAND
elseif ($argv[1] == 'backup') {
    backup();
}
# RESTORE COMMAND
elseif ($argv[1] == 'restore') {
    restore();
    sync();
}
# SYNC COMMAND
elseif ($argv[1] == 'sync') {
    sync();
}
# UPDATE COMMAND
elseif ($argv[1] == 'update') {
    update();
}
# REVERT COMMAND
elseif ($argv[1] == 'revert') {
    revert($argv[2]);
}
# DELETE COMMAND
elseif ($argv[1] == 'delete') {
    delete();
}
# ROTATESERVERKEY COMMAND
elseif ($argv[1] == 'rotateserverkey') {
    rotate_server_key();
}
# VERSION COMMAND
elseif ($argv[1] == 'version') {
    version();
}
# HELP COMMAND
elseif (in_array($argv[1], ['help', null])) {
    help();
}
# UNKNOWN COMMAND/DEFAULT
else {
    echo 'Error: Unknown command' . PHP_EOL . PHP_EOL;
    help();
}

exit();

