<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Auth;
use RESTAPI\Core\Model;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;

class SSH extends Model
{
    public BooleanField $enable;
    public PortField $port;
    public StringField $sshdkeyonly;
    public BooleanField $sshdagentforwarding;

    public function __construct(mixed $data = [], mixed ...$options)
    {
        # Set model attributes
        $this->config_path = "system/ssh";
        $this->always_apply = true;

        # Set model Fields
        $this->enable = new BooleanField(
            default: false,
            indicates_true: "enabled",
            help_text: "Enable the SSH server on this system."
        );
        $this->sshdagentforwarding = new BooleanField(
            default: false,
            indicates_true: "enabled",
            help_text: "Enable support for ssh-agent forwarding."
        );
        $this->port = new PortField(
            default: "22",
            allow_alias: false,
            allow_range: false,
            help_text: "The TCP port the SSH server will listen on."
        );
        $this->sshdkeyonly = new StringField(
            default: null,
            choices: ["enabled", "both"],
            allow_null: true,
            help_text: "The SSH authentication mode to use. Use `enabled` to require public key authentication, use ".
                "both to require both a public key AND a password, or use `null` to allow a password OR a public key."
        );

        # Construct the parent Model object
        parent::__construct(...$options, data: $data);
    }

    /**
     * Applies the SSH server changes.
     */
    public function apply() {
        # Stop the sshd service
        killbyname("sshd");
        $this->log_error("secure shell configuration has changed. Stopping sshd.");

        # If sshd is still enabled, restart the sshd service using the new config
        if (config_path_enabled('system/ssh', 'enable')) {
            $this->log_error("secure shell configuration has changed. Restarting sshd.");
            send_event("service restart sshd");
        }
    }
}
