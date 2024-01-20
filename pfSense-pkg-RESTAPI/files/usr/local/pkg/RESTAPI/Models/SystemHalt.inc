<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Command;
use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\SystemHaltDispatcher;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\IntegerField;

class SystemHalt extends Model
{
    public IntegerField $delay;
    public BooleanField $dry_run;

    public function __construct(mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->many = false;

        # Set model fields
        $this->dry_run = new BooleanField(
            default: false,
            write_only: true,
            help_text: "Run through the call but don't actually initiate a reboot."
        );

        parent::__construct(...$options, data: $data);
    }

    /**
     * Returns an empty array. This method is the internal callable for this Model
     */
    protected function void(): array {
        return [];
    }

    /**
     * Defines the steps necessary to initiate a reboot.
     */
    public function _create(): void
    {
        # Only initiate the reboot if this is not a dry run
        if (!$this->dry_run->value) {
            (new SystemHaltDispatcher($this->async))->spawn_process();
        }
    }
}