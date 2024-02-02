<?php

namespace RESTAPI\Core;

/**
 * Defines objects that execute shell commands and keep track of the output.
 * @property string $command The shell command this object should execute.
 * @property string $redirect An optional shell redirect to append to the end of the $command.
 * @property string $output The output of the executed shell command.
 * @property int $result_code The exit/return code of the executed shell command.
 * @property bool $trim_whitespace Automatically remove excessive whitespace from the command $output.
 */
class Command extends BaseClass
{
    public string $command;
    public string $redirect;
    public string $output = "";
    public int $result_code = -1;
    public bool $trim_whitespace = false;

    /**
     * Defines the Command object including the shell command to execute and optional modifiers. Note: By default,
     * the command output will redirect stderr to stdout so error message will be included in the output.
     * @param string $command The shell command to execute.
     * @param bool $trim_whitespace Remove excessive whitespace from the command output.
     * @param string $redirect An optional shell redirect to append to the end of the $command.
     * @return Command Returns this object containing the results of the executed command. Note: the object returned
     * cannot be used to initiate new commands. A new Command object should be created for any additional commands.
     */
    public function __construct(string $command, bool $trim_whitespace = false, string $redirect = "2>&1")
    {
        $this->command = $command;
        $this->trim_whitespace = $trim_whitespace;
        $this->redirect = $redirect;
        $this->run_command();
        return $this;
    }

    /**
     * Executes the assigned $command. The $output and $result_code properties will be set after running
     * this method.
     */
    private function run_command(): void
    {
        exec(command: "$this->command $this->redirect", output: $raw_output, result_code: $this->result_code);
        $this->output = implode(PHP_EOL, $raw_output);

        # Normalize output's whitespace if requested
        if ($this->trim_whitespace) {
            $this->output = preg_replace('/\s+/', ' ', $this->output);
        }
    }
}
