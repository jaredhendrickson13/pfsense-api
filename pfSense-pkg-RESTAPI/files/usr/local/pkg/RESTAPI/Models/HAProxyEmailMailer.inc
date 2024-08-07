<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\HAProxyApplyDispatcher;
use RESTAPI\Fields\PortField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\IPAddressValidator;

/**
 * Defines a Model for HAProxy Email Mailers.
 */
class HAProxyEmailMailer extends Model {
    public StringField $name;
    public StringField $mailserver;
    public PortField $mailserverport;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->parent_model_class = 'HAProxySettings';
        $this->config_path = 'email_mailers/item';
        $this->packages = ['pfSense-pkg-haproxy'];
        $this->package_includes = ['haproxy/haproxy.inc', 'haproxy/haproxy_utils.inc'];
        $this->many = true;

        # Set model fields
        $this->name = new StringField(
            required: true,
            unique: true,
            allow_empty: true,
            help_text: 'The descriptive name for this mail server.',
        );
        $this->mailserver = new StringField(
            required: true,
            validators: [new IPAddressValidator(allow_ipv4: true, allow_ipv6: true, allow_fqdn: true)],
            help_text: 'The IP or hostname of the mail server.',
        );
        $this->mailserverport = new PortField(
            default: '53',
            allow_alias: false,
            allow_range: false,
            help_text: 'The port used by this mail server.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Ensures haproxy is marked as dirty before applying.
     */
    public function pre_apply(): void {
        touch('/var/run/haproxy.conf.dirty');
    }

    /**
     * Applies changes to the HAProxy configuration.
     */
    public function apply(): void {
        (new HAProxyApplyDispatcher(async: $this->async))->spawn_process();
    }
}
