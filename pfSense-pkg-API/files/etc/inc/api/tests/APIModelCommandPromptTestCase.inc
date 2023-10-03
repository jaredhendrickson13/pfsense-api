<?php

namespace API\Tests;

use API\Core\Command;
use API\Core\TestCase;
use API\Models\CommandPrompt;

class APIModelCommandPromptTestCase extends TestCase
{
    /**
     * Checks that CommandPrompt correctly runs shell commands and provides the correct response.
     */
    public function test_command_prompt() {
        # Create a CommandPrompt model to test with
        $test_cmd = new CommandPrompt();

        # Set the command to run and ensure the command is correctly executed using the create() method.
        $test_cmd->command->value = "echo 'test'";
        $test_cmd->create();
        $this->assert_equals($test_cmd->output->value, "test");
        $this->assert_equals($test_cmd->result_code->value, 0);

        # Set another command to run and ensure the command is correctly executed using the create() method.
        $test_cmd->command->value = "not_an_executable";
        $test_cmd->create();
        $this->assert_equals($test_cmd->output->value, "sh: not_an_executable: not found");
        $this->assert_equals($test_cmd->result_code->value, 127);
    }
}
