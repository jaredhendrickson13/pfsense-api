<?php
require_once("api/framework/APIBaseModel.inc");
require_once("api/framework/APIResponse.inc");
require_once("api/api_models/APISystemDNS.inc");


class APISystemDNSDeleteServers extends APIBaseModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->methods = ["POST"];
        $this->privileges = ["page-all", "page-system"];
        $this->change_note = "Deleted system DNS servers via API";
    }

    public function action() {
        $this->write_config();    // Apply our configuration change
        // Update a slew of backend services
        system_resolvconf_generate();
        if (isset($this->config['dnsmasq']['enable'])) {
            services_dnsmasq_configure();
        } elseif (isset($this->config['unbound']['enable'])) {
            services_unbound_configure();
        }
        send_event("service reload dns");
        filter_configure();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        if (isset($this->initial_data['dnsserver'])) {
            $del_server = $this->initial_data['dnsserver'];
            $curr_servers = $this->config["system"]["dnsserver"];
            $del_server = (!is_array($del_server)) ? array($del_server) : $del_server;
            foreach ($del_server as $ds) {
                // Ensure our config is array
                if (!is_array($curr_servers)) {
                    $curr_servers = array($this->config["system"]["dnsserver"]);
                }
                // Loop through each server and check for matches, delete on match
                foreach ($curr_servers as $id => $cs) {
                    if ($ds === $cs) {
                        $this->validated_data["dnsserver"][] = $ds;
                        unset($this->config["system"]["dnsserver"][$id]);
                    }
                }
            }
        }
    }
}
