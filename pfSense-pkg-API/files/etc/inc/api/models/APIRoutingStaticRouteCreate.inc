<?php
require_once("api/framework/APIModel.inc");
require_once("api/framework/APIResponse.inc");


class APIRoutingStaticRouteCreate extends APIModel {
    # Create our method constructor
    public function __construct() {
        parent::__construct();
        $this->privileges = ["page-all", "page-system-staticroutes-editroute"];
        $this->change_note = "Added static route via API";
    }

    public function action() {
        if (!is_array($this->config["staticroutes"])) {
            $this->config["staticroutes"] = [];
            $this->config["staticroutes"]["route"] = [];
        }

        $this->config["staticroutes"]["route"][] = $this->validated_data;
        $this->write_config();
        $this->validated_data["id"] = count($this->config["staticroutes"]["route"]) - 1;
        system_routing_configure();
        filter_configure();
        setup_gateways_monitor();
        return APIResponse\get(0, $this->validated_data);
    }

    public function validate_payload() {
        # Validate our required 'network' payload value
        if (isset($this->initial_data['network'])) {
            $subnet = intval(explode("/", $this->initial_data["network"])[1]);
            $ip = explode("/", $this->initial_data["network"])[0];
            # Check if address is IPv4, IPv6 or alias. Otherwise return an error
            if (is_ipaddrv4($ip)) {
                # Ensure the subnet bit mask is within range for IPv4 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 32)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }

            }
            elseif (is_ipaddrv6($ip)) {
                # Ensure the subnet bit mask is within range for IPv6 addresses
                if (is_numeric($subnet) and ($subnet >= 1 and $subnet <= 128)) {
                    $this->validated_data["network"] = $this->initial_data["network"];
                } else {
                    $this->errors[] = APIResponse\get(6002);
                }
            } elseif (is_alias($ip)) {
                $this->validated_data["network"] = $this->initial_data["network"];
            } else {
                $this->errors[] = APIResponse\get(6001);
            }
        } else {
            $this->errors[] = APIResponse\get(6000);
        }

        # Validate our required 'gateway' payload value
        if (isset($this->initial_data["gateway"])) {
            # Ensure this is a valid gateway
            if (APITools\is_gateway($this->initial_data["gateway"])) {
                $this->validated_data["gateway"] = $this->initial_data["gateway"];
            } else {
                $this->errors[] = APIResponse\get(6004);
            }
        } else {
            $this->errors[] = APIResponse\get(6003);
        }

        # Validate our optional 'descr' payload value
        if (isset($this->initial_data["descr"])) {
            $this->validated_data["descr"] = strval($this->initial_data["descr"]);
        } else {
            $this->validated_data["descr"] = "";
        }

        # Validate our optional 'disabled' payload value
        if ($this->initial_data["disabled"] == true ) {
            $this->validated_data["disabled"] = "";
        }

    }

}