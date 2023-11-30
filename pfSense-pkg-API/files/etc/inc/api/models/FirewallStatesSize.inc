<?php

namespace API\Models;

use API\Core\Auth;
use API\Core\Model;
use API\Fields\IntegerField;

class FirewallStatesSize extends Model
{
    public IntegerField $maximumstates;
    public IntegerField $defaultmaximumstates;
    public IntegerField $currentstates;
    
    public function __construct(mixed $data = [], Auth $client = null, bool $async = true, ...$vl_data)
    {
        # Set model attributes
        $this->config_path = "system";
        $this->update_strategy = "merge";
        
        # Set model fields
        $this->maximumstates = new IntegerField(
            required: true,
            minimum: 10,
            help_text: "The maximum number of firewall state entries allowed by this firewall."
        );
        $this->defaultmaximumstates = new IntegerField(
            read_only: true,
            help_text: "The default number of firewall state entries allowed by this firewall."
        );
        $this->currentstates = new IntegerField(
            read_only: true,
            help_text: "The number of firewall state entries currently registered in the states table."
        );
        parent::__construct(data: $data, client: $client, async: $async, vl_data: $vl_data);
    }
    
    /**
     * Obtains the default maximum states value from internal pfSense functions.
     */
    public function from_internal_defaultmaximumstates() {
        return pfsense_default_state_size();
    }

    /**
     * Obtains the current number of states value from internal pfSense functions.
     */
    public function from_internal_currentstates() {
        return explode("/", get_pfstate())[0];
    }

}
