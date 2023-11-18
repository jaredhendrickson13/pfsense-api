<?php

namespace API\Core;

use API\Responses\ServerError;
use API\Responses\ServiceUnavailableResponse;

/**
 * The Dispatcher object is used to define functions that are intended to be run in the background. The Dispatcher
 * objects allows us to dynamically create a private PHP script that can be called in the background, manages the
 * process spawn queue, and enforces background process timeouts.
 */
abstract class Dispatcher
{
    private string $name;
    private string $pid_dir;
    private string $pid_file;
    private string $pid_file_prefix;
    private string $script_file;
    public int $timeout = 600;
    public int $delay = 0;
    public int $max_concurrency = 0;

    public function __construct()
    {
        # Get the current class name including namespace, remove slashes
        $this->name = str_replace("\\", "", get_called_class());
        
        # Set the Dispatcher PID file directory and name. This cannot be changed by child classes.
        $this->pid_dir = "/tmp/";
        $this->pid_file_prefix = $this->name."-";
        $this->pid_file = $this->pid_dir.uniqid(prefix: $this->pid_file_prefix).".pid";

        # Set the path of the Dispatcher script. This will be used when building the Dispatcher and spawning processes
        $this->script_file = "/usr/local/share/pfSense-pkg-API/dispatchers/".$this->name.".php";
    }

    /**
     * Builds the Dispatcher PHP script at the assigned filepath.
     */
    public function build_dispatcher_script() {
        # Get the fully qualified class name for this object
        $fq_class_name = get_class($this);

        # Specify the PHP code to write to the Dispatcher script file
        $code = "<?php
        require_once('api/auto_loader.inc');
        (new ".$fq_class_name."())->process();";

        # Assign the absolute path to the file. Assume index.php filename if not specified.
        echo "Building ".$fq_class_name." at path $this->script_file... ";
        mkdir(dirname($this->script_file), 0755, true);
        file_put_contents($this->script_file, $code);

        # Print success output if file now exists, otherwise output error and exit on non-zero code
        if (is_file($this->script_file)) {
            echo "done.".PHP_EOL;
        }
        else {
            echo "failed.".PHP_EOL;
            exit(1);
        }
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
     * Spawns a new process for this Dispatcher.
     * @param mixed ...$arguments Optional arguments to pass to the `process()` method.
     * @return int The PID of the spawned process.
     */
    public function spawn_process(mixed ...$arguments) : int {
        # Before we spawn a new process, ensure we don't have too many concurrent processes running now
        if ($this->max_concurrency and count($this->get_running_processes()) >= $this->max_concurrency) {
            throw new ServiceUnavailableResponse(
                message: "Dispatcher allows for a maximum of `$this->max_concurrency` processes to be running at ".
                    "once, try again after `$this->timeout` seconds.",
                response_id: "DISPATCHER_TOO_MANY_CONCURRENT_PROCESSES",
                retry_after: $this->timeout
            );
        }
        
        # Delay spawning the process if a delay was requested.
        if ($this->delay) {
            sleep($this->delay);
        }
        
        # Spawn the process
        $spawn_process = new Command(
            command: "nohup timeout $this->timeout php -f $this->script_file",
            redirect: ">/dev/null & echo $!"
        );
        $pid = (is_numeric($spawn_process->output)) ? (int)$spawn_process->output : null;
        
        # Ensure the spawn process output is a numeric PID and that the PID is actively running
        if (!is_null($pid) and posix_getpgid($pid)) {
            # Write this Dispatcher run's PID file and return the PID.
            file_put_contents($this->pid_file, $pid);
            return $pid;
        }
        
        # Throw an error if we failed to spawn the Dispatcher process
        throw new ServerError(
            message: "Dispatcher `$this->name` failed to spawn new process. Received: $spawn_process->output",
            response_id: "DISPATCHER_FAILED_TO_SPAWN_PROCESS"
        );
    }

    /**
     * Defines what should be done when the Dispatch process is spawned. In most cases, this will restart some service
     * or perform computations that may take a long time. It is up to the child Dispatcher class to decide what is
     * done here.
     */
    public static abstract function process(mixed ...$arguments);
}
