<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\DHCPServerApplyDispatcher;
use RESTAPI\Fields\StringField;

class DHCPServerBackend extends Model
{
    public StringField $dhcpbackend;

    public function __construct(mixed $data = [], ...$options)
    {
        # Set model attributes
        $this->config_path = "/";
        $this->many = false;
        $this->update_strategy = "merge";
        $this->always_apply = true;

        # Set model fields
        $this->dhcpbackend = new StringField(
            default: "isc",
            choices: ["isc", "kea"],
            help_text: "The backend DHCP server service to use. ISC DHCP is deprecate and will be removed in a future ".
                "version of pfSense."
        );

        parent::__construct(...$options, data: $data);
    }
    
    /**
     * Apply changes to the DHCP server service.
     */
    public function apply(): void
    {
        (new DHCPServerApplyDispatcher(async: $this->async))->spawn_process();
    }
}
