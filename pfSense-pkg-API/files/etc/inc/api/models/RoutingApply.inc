<?php

namespace API\Models;

use API;
use API\Core\Auth;
use API\Core\Model;
use API\Fields\BooleanField;

/**
 * Defines a Model that provides details on pending routing changes and allows pending routing changes to be applied.
 */
class RoutingApply extends Model
{
    public BooleanField $applied;

    public function __construct(mixed $id = null, mixed $representation_data = [], Auth $client = null)
    {
        # Set model attributes
        $this->internal_callable = "get_applied_status";
        $this->always_apply = true;

        # Set model fields
        $this->applied = new BooleanField(
            read_only: true,
            indicates_true: true,
            indicates_false: false,
            help_text: "Displays `true` when all routing changes are applied and there are no pending changes left.".
            "Displays `false` when there are pending routing changes that have not been applied."
        );
        parent::__construct($id, $representation_data, $client);
    }

    /**
     * Obtains the current applied status.
     * @return array The internal value array to be loaded by this model as the internal callable
     */
    public function get_applied_status() : array {
        return ["applied" => !is_subsystem_dirty("staticroutes")];
    }

    /**
     * Override the _create method so it does nothing. We only want to apply changes when this is called.
     */
    public function _create(){
    }

    /**
     * Applies pending routing changes.
     */
    public function apply() {
        global $g;

        # Check for the pending changes file and unserialize it
        if (file_exists("{$g['tmp_path']}/.system_routes.apply")) {
            $to_apply_list = unserialize(file_get_contents("{$g['tmp_path']}/.system_routes.apply"));

            # Run commands to apply these changes
            foreach ($to_apply_list as $to_apply) {
                mwexec("{$to_apply}");
            }

            @unlink("{$g['tmp_path']}/.system_routes.apply");
        }

        # Reload associated services
        system_routing_configure();
        system_resolvconf_generate();
        filter_configure();
        setup_gateways_monitor();
        send_event("service reload dyndnsall");
        clear_subsystem_dirty("staticroutes");
    }
}