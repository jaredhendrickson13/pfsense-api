<?php
//   Copyright 2023 Jared Hendrickson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");

function unbound_override_flush_host_cmp($a, $b) {
    return strcasecmp($a['host'], $b['host']);
}

class APIServicesUnboundHostOverrideFlushUpdate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->change_note = "Flushed and replaced DNS Resolver host overrides via API";
    }

    public function action() {
        # Replace any existing host overrides with the host overrides validated in this request
        $this->set_config('unbound/hosts', $this->validated_data);
        $hosts = $this->get_config("unbound/hosts", []);
        usort($hosts, "unbound_override_flush_host_cmp");
        $this->write_config();

        # Mark the Unbound subsystem as changed.
        mark_subsystem_dirty("unbound");

        # Apply the changes if requested
        if ($this->initial_data["apply"] === true) {
            (new APIServicesUnboundApplyCreate)->action();
        }

        return APIResponse\get(0, $hosts);
    }

    public function validate_payload() {
        # Require data to be passed in as an array
        if (is_array($this->initial_data["host_overrides"])) {
            # Loop through each host override entry requested
            foreach ($this->initial_data["host_overrides"] as $initial_host_override) {
                # Check if this entry is valid for creation using the APIServicesUnboundHostOverrideCreate class
                $ent = new APIServicesUnboundHostOverrideCreate();
                $ent->initial_data = $initial_host_override;
                $ent->validate_payload();

                # Check if an occurred while validating this entry
                if ($ent->errors) {
                    # Grab the error's return code and raise error including the bad entry in the data field
                    $rc = $ent->errors[0]["return"];
                    $this->errors[] = APIResponse\get($rc, $initial_host_override);
                    break;
                } # Otherwise, if the entry was valid, add it to our validated host overrides
                else {
                    $this->validated_data[] = $ent->validated_data;
                }
            }
        }
        # Raise an error if host overrides were not passed in as an array
        else {
            $this->errors[] = APIResponse\get(2098);
        }
    }
}
