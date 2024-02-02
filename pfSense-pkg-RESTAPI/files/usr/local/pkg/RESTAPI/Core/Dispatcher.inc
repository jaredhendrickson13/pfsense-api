<?php

namespace RESTAPI\Core;

use ReflectionClass;
use RESTAPI\Models\CronJob;
use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\ServiceUnavailableError;

/**
 * The Dispatcher object is used to define functions that are intended to run in the background. The Dispatcher
 * objects allows us to dynamically create a private PHP script that can be called in the background, manages the
 * process spawn queue, and enforces background process timeouts.
 */
class Dispatcher
{
    use BaseTraits;
    
    /**
     * @const DISPATCH_SCRIPT The absolute file path to the dispatch.sh helper script.
     */
    const DISPATCH_SCRIPT = '/usr/local/pkg/RESTAPI/.resources/scripts/dispatch.sh';
    /**
     * @const SCHEDULE_OPTIONS The cron event schedules supported by Dispatchers.
     */
    const SCHEDULE_OPTIONS = ['hourly', 'daily', 'weekly'];

    /**
     * @var string $full_name
     * The full name of this Dispatcher. This value is automatically populated using the class
     * FQN, but all slashes are removed. This value is used when reading and writing the Dispatcher's PID file.
     */
    protected string $full_name;
    /**
     * @var string $short_name
     * The Dispatcher class's short name. This value is passed to the dispatch.sh helper
     * script to indicate which Dispatcher class should be run.
     */
    protected string $short_name;
    /**
     * @var string $pid_dir
     * Sets the directory where the Dispatcher PID file will be written. This file name must be
     *  unique for each Dispatcher process spawned.
     */
    protected string $pid_dir;
    /**
     * @var string $pid_file
     * Sets the entire name of the PID file.
     */
    protected string $pid_file;
    /**
     * @var string $pid_file_prefix The PID file name prefix. This value should contain this Dispatcher's $full_name.
     */
    protected string $pid_file_prefix;
    /**
     * @var int $timeout
     * Sets the maximum number of seconds that processes spawned by this Dispatcher can run for.
     * After this time-frame, the process will be terminated. This ensures Dispatchers cannot spawn processes that run
     * endlessly and crash the system.
     */
    public int $timeout = 600;
    /**
     * @var int $max_concurrency
     * Sets the maximum number of processes this Dispatcher can have running at one time.
     * Attempts to spawn process past this limit will result in a ServiceUnavailableError error.
     */
    public int $max_concurrency = 0;
    /**
     * @var bool $async
     * Indicates whether this Dispatcher object should spawn processes asynchronously (in the
     * background) or synchronously (waits for process to complete). If set to `true`, any `spawn_process()` method calls
     * will spawn the process in the background and immediately respond. If set to `false`, any `spawn_process()` will
     * wait until the process finishes to respond.
     */
    public bool $async = true;
    /**
     * @var string $schedule
     * Specifies the frequency in which this Dispatcher should be run on a schedule. Valid options are `hourly`,
     * `daily`, and `weekly`. Leave blank if this Dispatcher should not automatically run on a schedule.
     */
    public string $schedule = '';
    /**
     * @var string $schedule_command
     * The shell command that will be used to call this Dispatcher on a schedule.
     */
    protected string $schedule_command = '';

    /**
     * Constructs the Dispatcher object.
     * @param bool $async Set to `false` to prevent this Dispatcher from running the process in the background.
     */
    public function __construct(bool $async = true)
    {
        # Get the current full and short class names
        $this->full_name = str_replace('\\', '', get_called_class());
        $this->short_name = $this->get_class_shortname();

        # Set the Dispatcher PID file directory and name. This cannot be changed by child classes.
        $this->pid_dir = '/tmp/';
        $this->pid_file_prefix = $this->full_name . '-';
        $this->pid_file = $this->pid_dir . uniqid(prefix: $this->pid_file_prefix) . '.pid';

        # Set the Dispatcher async property
        $this->async = $async;

        # Set the schedule command for this Dispatcher
        $this->schedule_command = "/usr/local/pkg/RESTAPI/.resources/scripts/manage.php notifydispatcher $this->short_name";
    }

    /**
     * Reads the PID file path that will be used for the next spawned process.
     */
    public function get_pid_file(): string
    {
        return $this->pid_file;
    }

    /**
     * Obtains the PIDs of any active processes spawned by this Dispatcher.
     * @returns array An array of PIDs of processes spawned by this Dispatcher.
     */
    public function get_running_processes(): array
    {
        # Variables
        $pids = [];

        # Loop through each existing proc file and checks its PID
        foreach (glob($this->pid_dir . $this->pid_file_prefix . '*.pid') as $pid_file) {
            $pid = (int) file_get_contents($pid_file);

            # If this PID is actively running, add it to the return array
            if (posix_getpgid($pid)) {
                $pids[] = $pid;
            }
            # Otherwise, remove the PID file
            else {
                unlink($pid_file);
            }
        }

        return $pids;
    }

    /**
     * Kills any running Dispatcher processes.
     */
    public function kill_running_processes(): void
    {
        # Terminate this Dispatcher's running processes
        foreach ($this->get_running_processes() as $pid) {
            posix_kill($pid, SIGKILL);
        }

        # Remove any leftover PID files
        foreach (glob($this->pid_dir . $this->pid_file_prefix . '*.pid') as $pid_file) {
            unlink($pid_file);
        }
    }

    /**
     * Spawns a new process for this Dispatcher.
     * @param mixed ...$arguments Optional arguments to pass to the `process()` method.
     * @return int The PID of the spawned process.
     * @throws ServiceUnavailableError When the maximum number of processes allowed by $max_concurrency is exceeded.
     */
    public function spawn_process(mixed ...$arguments): int
    {
        # Before we spawn a new process, ensure we don't have too many concurrent processes running now
        if ($this->max_concurrency and count($this->get_running_processes()) >= $this->max_concurrency) {
            throw new ServiceUnavailableError(
                message: "Dispatcher allows for a maximum of `$this->max_concurrency` processes to be running at " .
                    "once, try again after `$this->timeout` seconds.",
                response_id: 'DISPATCHER_TOO_MANY_CONCURRENT_PROCESSES',
                retry_after: $this->timeout,
            );
        }

        # Spawn the process in the background if this Dispatcher is asynchronous
        if ($this->async) {
            $spawn_process = new Command(
                command: 'nohup sh ' . self::DISPATCH_SCRIPT . " $this->short_name $this->pid_file $this->timeout",
                redirect: ">/dev/null & echo $!",
            );
            $pid = is_numeric($spawn_process->output) ? (int) $spawn_process->output : null;

            # Ensure the spawn process output is a numeric PID and that the PID is actively running
            if (!is_null($pid) and posix_getpgid($pid)) {
                # Write this process's PID file, set the PID file name for the next process and return the PID.
                file_put_contents($this->pid_file, $pid);
                $this->pid_file = $this->pid_dir . uniqid(prefix: $this->pid_file_prefix) . '.pid';
                return $pid;
            }

            # Throw an error if we failed to spawn the Dispatcher process
            throw new ServerError(
                message: "Dispatcher `$this->full_name` failed to spawn new process. Received: $spawn_process->output",
                response_id: 'DISPATCHER_FAILED_TO_SPAWN_PROCESS',
            );
        }

        # Otherwise, call the `process()` directly and wait for it to complete
        # Since we are running this process synchronously, use this PHP fork's PID
        $pid = getmypid();
        file_put_contents($this->pid_file, $pid);

        # Call the process directly and remove the PID file after its done
        $this->process();
        unlink($this->pid_file);

        # Set the PID file name for the next process and return the PID.
        $this->pid_file = $this->pid_dir . uniqid(prefix: $this->pid_file_prefix) . '.pid';
        return $pid;
    }

    /**
     * Defines what should be done when the Dispatch process is spawned. In most cases, this will restart some service
     * or perform computations that may take a long time. It is up to the child Dispatcher class to decide what is
     * done here.
     */
    public function process(mixed ...$arguments): void
    {
        # Emulate a long running process for testing purposes
        sleep(30);
    }

    /**
     * Logs an error to the syslog.
     * @param string $message The error message to write to the syslog
     */
    public static function log_error(string $message): void
    {
        # Call the pfSense `log_error` function
        log_error($message);
    }

    /**
     * Configures this Dispatcher to run on a schedule if the `schedule` property is set.
     * @return CronJob|null Returns the CronJob created for this Dispatcher if a `schedule` is defined. Returns `null`
     * if no schedule was defined.
     */
    public function setup_schedule(): CronJob|null
    {
        # Only proceed if a schedule was requested
        if ($this->schedule) {
            # Ensure the requested schedule is supported
            if (!in_array($this->schedule, self::SCHEDULE_OPTIONS)) {
                throw new ServerError(
                    message: "Dispatcher schedule `$this->schedule` is not a supported schedule frequency.",
                    response_id: 'DISPATCHER_SCHEDULE_UNSUPPORTED',
                );
            }

            # Check if a cron job already exists for this dispatcher
            $dispatcher_cron_job_q = CronJob::query(command: $this->schedule_command);

            # Delete the cron job for this dispatcher if it exists, so we can recreate it with current values
            if ($dispatcher_cron_job_q->exists()) {
                $existing_cron_job = $dispatcher_cron_job_q->first();
                $existing_cron_job->packages = []; // Don't require the pfSense-pkg-Cron package to delete
                $existing_cron_job->delete();
            }

            # Create the cron job for this dispatcher
            $cron_job = new CronJob(
                data: ['minute' => "@$this->schedule", 'who' => 'root', 'command' => $this->schedule_command],
                require_pkg: false,
            );
            $cron_job->create();

            # Spawn this dispatcher process to ensure applicable values are populated before the next schedule runs
            $this->spawn_process();

            # Return the created CronJob
            return $cron_job;
        }

        return null;
    }
}
