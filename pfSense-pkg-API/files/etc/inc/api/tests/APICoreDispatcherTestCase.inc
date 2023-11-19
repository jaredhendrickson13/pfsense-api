<?php

namespace API\Tests;

use API\Core\Dispatcher;
use API\Core\TestCase;

class APICoreDispatcherTestCase extends TestCase
{
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
            response_id: "DISPATCHER_TOO_MANY_CONCURRENT_PROCESSES",
            code: 503,
            callable: function () use ($dispatcher) {
                $dispatcher->spawn_process();
            }
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
        $this->assert_equals((int)file_get_contents($pid_file), $pid);

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

}