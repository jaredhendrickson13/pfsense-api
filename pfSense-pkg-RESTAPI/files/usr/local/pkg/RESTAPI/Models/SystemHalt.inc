<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\SystemHaltDispatcher;
use RESTAPI\Fields\BooleanField;

/**
 * Defines a Model that performs a system halt operation.
 */
class SystemHalt extends Model {
    public BooleanField $dry_run;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->many = false;

        # Set model fields
        $this->dry_run = new BooleanField(
            default: false,
            write_only: true,
            help_text: "Run through the call but don't actually initiate a shutdown.",
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Returns an empty array. This method is the internal callable for this Model
     */
    protected function void(): array {
        return [];
    }

    /**
     * Defines the steps necessary to initiate a shutdown.
     */
    public function _create(): void {
        # Only initiate the shutdown if this is not a dry run
        if (!$this->dry_run->value) {
            (new SystemHaltDispatcher($this->async))->spawn_process();
        }
    }
}
