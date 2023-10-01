<?php

namespace API\Models;

use API;
use API\Core\Command;
use API\Core\Model;
use API\Fields\IntegerField;
use API\Fields\StringField;

/**
 * Defines a Model object that can be used to run shell commands and retrieve the output and result code.
 */
class CommandPrompt extends Model
{
    public StringField $command;
    public StringField $output;
    public IntegerField $result_code;
    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Define Model fields
        $this->command = new StringField(
            required: true,
            write_only: true,
            help_text: "The command to be executed."
        );
        $this->output = new StringField(
            allow_null: true,
            read_only: true,
            help_text: "The output of the executed command."
        );
        $this->result_code = new IntegerField(
            allow_null: true,
            read_only: true,
            help_text: "The result code of the executed command."
        );

        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Execute the requested command and populate the output and result code.
     */
    public function _create(): void
    {
        $cmd = new Command($this->command->value);
        $this->output->value = $cmd->output;
        $this->result_code->value = $cmd->result_code;
    }
}