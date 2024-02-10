<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Core\TestCase;
use RESTAPI\Dispatchers\WireGuardSettingsDispatcher;
use RESTAPI\Models\Package;

class APICoreDispatcherTestCase extends TestCase {
    /**
     * Checks that the `max_concurrency` property limits the number of active Dispatcher process that can
     * be spawn.
     */
    public function test_max_concurrency() {
        # Create a test dispatcher and spawn the maximum number of processes
        $dispatcher = new Dispatcher();
        $dispatcher->max_concurrency = 2;
        $dispatcher->spawn_process();
        $dispatcher->spawn_process();

        # Ensure an error is thrown trying to spawn another
        $this->assert_throws_response(
            response_id: 'DISPATCHER_TOO_MANY_CONCURRENT_PROCESSES',
            code: 503,
            callable: function () use ($dispatcher) {
                $dispatcher->spawn_process();
            },
        );

        # Kill the running processes
        $dispatcher->kill_running_processes();
    }

    /**
     * Checks that running processes are correctly identified by `get_running_processes()`
     */
    public function test_get_running_processes() {
        # Create a test dispatcher
        $dispatcher = new Dispatcher();

        # Spawn a few processes
        $pid1 = $dispatcher->spawn_process();
        $pid2 = $dispatcher->spawn_process();

        # Ensure both spawned PIDs are in the returned array
        $this->assert_equals($dispatcher->get_running_processes(), [$pid1, $pid2]);

        # Kill the running processes
        $dispatcher->kill_running_processes();
    }

    /**
     * Checks that the PID file is created after process spawns.
     */
    public function test_pid_file_exists_after_spawn() {
        # Create a test dispatcher
        $dispatcher = new Dispatcher();
        $dispatcher->timeout = 2;
        $pid_file = $dispatcher->get_pid_file();

        # Spawn the process and ensure the PID file exists and it's contents match the PID
        $pid = $dispatcher->spawn_process();
        $this->assert_is_true(is_file($pid_file));
        $this->assert_equals((int) file_get_contents($pid_file), $pid);

        # Kill the processes
        $dispatcher->kill_running_processes();
    }

    /**
     * Checks that the PID and PID file are killed when `kill_running_processes()` is called
     */
    public function test_kill_running_processes() {
        # Create a test dispatcher
        $dispatcher = new Dispatcher();
        $dispatcher->timeout = 2;
        $pid_file = $dispatcher->get_pid_file();

        # Spawn the process and ensure the PID file exists and it's contents match the PID
        $pid = $dispatcher->spawn_process();

        # Kill the processes and ensure the PID and PID file are no longer present
        $dispatcher->kill_running_processes();
        $this->assert_is_false(is_file($pid_file));
        $this->assert_is_empty($dispatcher->get_running_processes());
    }

    /**
     * Checks that Dispatcher processes is correctly timed out after the specified amount of time.
     */
    public function test_process_timeout() {
        # Create a test dispatcher
        $dispatcher = new Dispatcher();
        $dispatcher->timeout = 2;
        $pid_file = $dispatcher->get_pid_file();

        # Spawn a new process and ensure it is running
        $dispatcher->spawn_process();
        $this->assert_is_not_empty($dispatcher->get_running_processes());

        # Wait for the Dispatcher to timeout and check that the process is no longer running
        sleep($dispatcher->timeout + 1);
        $this->assert_is_empty($dispatcher->get_running_processes());

        # Ensure the PID file for the process is no longer present
        $this->assert_is_false(is_file($pid_file));
    }

    /**
     * Checks that a Dispatcher cannot be assigned an unsupported `schedule` value
     */
    public function test_schedule_options(): void {
        $this->assert_throws_response(
            response_id: 'DISPATCHER_SCHEDULE_UNSUPPORTED',
            code: 500,
            callable: function () {
                $dispatcher = new Dispatcher();
                $dispatcher->schedule = 'invalid_option';
                $dispatcher->setup_schedule();
            },
        );
    }

    /**
     * Checks that the Dispatcher's `setup_schedule()` method creates a CronJob for the Dispatcher using the `schedule`
     * property.
     */
    public function test_setup_schedule(): void {
        # Create a Dispatcher to test with but do not assign it a schedule yet
        $dispatcher = new Dispatcher();

        # Ensure the setup_schedule() returns no when no schedule is set
        $this->assert_equals($dispatcher->setup_schedule(), null);

        # Assign the dispatcher a schedule and ensure setup_schedule() returns a CronJob object with the correct schedule
        $dispatcher->schedule = 'daily';
        $dispatcher_cron_job = $dispatcher->setup_schedule();
        $cron_job_cmd = '/usr/local/pkg/RESTAPI/.resources/scripts/manage.php notifydispatcher Dispatcher';
        $this->assert_equals($dispatcher_cron_job->minute->value, "@$dispatcher->schedule");
        $this->assert_equals($dispatcher_cron_job->command->value, $cron_job_cmd);

        # Delete the CronJob
        $dispatcher_cron_job->delete();
    }

    /**
     * Checks that Dispatchers with $required_packages must have the listed packages installed before use.
     */
    public function check_required_packages(): void {
        # Ensure an error is thrown if a Dispatcher is constructed without all required packages installed
        $this->assert_throws_response(
            response_id: 'DISPATCHER_MISSING_REQUIRED_PACKAGE',
            code: 424,
            callable: function () {
                # Use the WireGuardSettingsDispatcher to test this as it requires pfSense-pkg-WireGuard.
                new WireGuardSettingsDispatcher();
            },
        );

        # Install the required package and ensure the error is no longer thrown when constructed
        $package = new Package(name: 'pfSense-pkg-WireGuard');
        $package->create();
        $this->assert_does_not_throw(
            callable: function () {
                # Use the WireGuardSettingsDispatcher to test this as it requires pfSense-pkg-WireGuard.
                $dispatcher = new WireGuardSettingsDispatcher();
                $dispatcher->spawn_process();
            },
        );

        # Remove the installed package
        $package->delete();
    }
}
