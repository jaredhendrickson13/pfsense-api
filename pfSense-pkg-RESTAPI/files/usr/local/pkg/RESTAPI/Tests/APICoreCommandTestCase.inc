<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\Command;
use RESTAPI\Core\TestCase;

class APICoreCommandTestCase extends TestCase
{
    /**
     * Checks that the Command object successfully runs the requested command and returns the correct output and
     * result code.
     */
    public function test_command()
    {
        # Run a command that doesn't exist, ensure it's output and result code are expected
        $cmd = new Command('asdf');
        $this->assert_equals($cmd->output, 'sh: asdf: not found');
        $this->assert_equals($cmd->result_code, 127);

        # Run an echo command, ensure it's output and result code are expected
        $cmd = new Command("echo 'test'");
        $this->assert_equals($cmd->output, 'test');
        $this->assert_equals($cmd->result_code, 0);
    }
}
