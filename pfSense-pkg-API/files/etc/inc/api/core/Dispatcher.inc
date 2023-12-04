<?php

namespace API\Core;

use API\Responses\ServerError;
use API\Responses\ServiceUnavailableError;
use ReflectionClass;

/**
 * The Dispatcher object is used to define functions that are intended to be run in the background. The Dispatcher
 * objects allows us to dynamically create a private PHP script that can be called in the background, manages the
 * process spawn queue, and enforces background process timeouts.
 * @const DISPATCH_SCRIPT The absolute file path to the dispatch.sh helper script.
 * @property string $full_name The full name of this Dispatcher. This value is automatically populated using the class
 * FQN, but all slashes are removed. This value is used when reading and writing the Dispatcher's PID file.
 * @property string $short_name The Dispatcher class's short name. This value is passed to the dispatch.sh helper
 * script to indicate which Dispatcher class should be run.
 * @property string $pid_dir Sets the directory where the Dispatcher PID file will be written. This file name must be
 * unique for each Dispatcher process spawned.
 * @property string $pid_file_prefix The PID file name prefix. This value should contain this Dispatcher's $full_name.
 * @property string $pid_file Sets the entire name of the PID file.
 * @property int $timeout Sets the maximum number of seconds that processes spawned by this Dispatcher can run for.
 * After this time-frame, the process will be terminated. This ensures Dispatchers cannot spawn processes that run
 * endlessly and crash the system.
 * @property int $max_concurrency Sets the maximum number of processes this Dispatcher can have running at one time.
 * Attempts to spawn process past this limit will result in a ServiceUnavailableError error.
 * @property bool $async Indicates whether this Dispatcher object should spawn processes asynchronously (in the
 * background) or synchronously (waits for process to complete). If set to `true`, any `spawn_process()` method calls
 * will spawn the process in the background and immediately respond. If set to `false`, any `spawn_process()` will
 * wait until the process finishes to respond.
 */
class Dispatcher
{
    const DISPATCH_SCRIPT = "/usr/local/share/pfSense-pkg-API/scripts/dispatch.sh";
    private string $full_name;
    private string $short_name;
    private string $pid_dir;
    private string $pid_file;
    private string $pid_file_prefix;
    public int $timeout = 600;
    public int $max_concurrency = 0;
    public bool $async = true;

    /**
     * Constructs the Dispatcher object.
     * @param bool $async Set to `false` to prevent this Dispatcher from running the process in the background.
     */
    public function __construct(bool $async = true)
    {
        # Get the current full and short class names
        $this->full_name = str_replace("\\", "", get_called_class());
        $this->short_name = (new ReflectionClass($this))->getShortName();
        
        # Set the Dispatcher PID file directory and name. This cannot be changed by child classes.
        $this->pid_dir = "/tmp/";
        $this->pid_file_prefix = $this->full_name."-";
        $this->pid_file = $this->pid_dir.uniqid(prefix: $this->pid_file_prefix).".pid";

        # Set the Dispatcher async property
        $this->async = $async;
    }

    /**
     * Reads the PID file path that will be used for the next spawned process.
     */
    public function get_pid_file() {
        return $this->pid_file;
    }

    /**
     * Obtains the PIDs of any active processes spawned by this Dispatcher.
     * @returns array An array of PIDs of processes spawned by this Dispatcher.
     */
    public function get_running_processes() : array {
        # Variables
        $pids = [];
        
        # Loop through each existing proc file and checks its PID
        foreach (glob($this->pid_dir.$this->pid_file_prefix."*.pid") as $pid_file) {
            $pid = (int)file_get_contents($pid_file);
            
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
    public function kill_running_processes() {
        # Terminate this Dispatcher's running processes
        foreach ($this->get_running_processes() as $pid) {
            posix_kill($pid, SIGKILL);
        }

        # Remove any leftover PID files
        foreach (glob($this->pid_dir.$this->pid_file_prefix."*.pid") as $pid_file) {
            unlink($pid_file);
        }
    }

    /**
     * Spawns a new process for this Dispatcher.
     * @param mixed ...$arguments Optional arguments to pass to the `process()` method.
     * @return int The PID of the spawned process.
     * @throws ServiceUnavailableError When the maximum number of processes allowed by $max_concurrency is exceeded.
     */
    public function spawn_process(mixed ...$arguments) : int {
        # Before we spawn a new process, ensure we don't have too many concurrent processes running now
        if ($this->max_concurrency and count($this->get_running_processes()) >= $this->max_concurrency) {
            throw new ServiceUnavailableError(
                message: "Dispatcher allows for a maximum of `$this->max_concurrency` processes to be running at ".
                    "once, try again after `$this->timeout` seconds.",
                response_id: "DISPATCHER_TOO_MANY_CONCURRENT_PROCESSES",
                retry_after: $this->timeout
            );
        }
        
        # Spawn the process in the background if this Dispatcher is asynchronous
        if ($this->async) {
            $spawn_process = new Command(
                command: "nohup sh ".self::DISPATCH_SCRIPT." $this->short_name $this->pid_file $this->timeout",
                redirect: ">/dev/null & echo $!"
            );
            $pid = (is_numeric($spawn_process->output)) ? (int)$spawn_process->output : null;

            # Ensure the spawn process output is a numeric PID and that the PID is actively running
            if (!is_null($pid) and posix_getpgid($pid)) {
                # Write this process's PID file, set the PID file name for the next process and return the PID.
                file_put_contents($this->pid_file, $pid);
                $this->pid_file = $this->pid_dir.uniqid(prefix: $this->pid_file_prefix).".pid";
                return $pid;
            }

            # Throw an error if we failed to spawn the Dispatcher process
            throw new ServerError(
                message: "Dispatcher `$this->full_name` failed to spawn new process. Received: $spawn_process->output",
                response_id: "DISPATCHER_FAILED_TO_SPAWN_PROCESS"
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
        $this->pid_file = $this->pid_dir.uniqid(prefix: $this->pid_file_prefix).".pid";
        return $pid;
    }

    /**
     * Defines what should be done when the Dispatch process is spawned. In most cases, this will restart some service
     * or perform computations that may take a long time. It is up to the child Dispatcher class to decide what is
     * done here.
     */
    public static function process(mixed ...$arguments) {
        # Emulate a long running process for testing purposes
        sleep(30);
    }
}
