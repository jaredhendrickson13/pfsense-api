<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\HAProxyApplyDispatcher;
use RESTAPI\Fields\BooleanField;
use RESTAPI\Fields\StringField;
use RESTAPI\Validators\RegexValidator;

/**
 * Defines a Model for HAProxy Frontend Access Control Lists.
 */
class HAProxyFrontendACL extends Model {
    public StringField $name;
    public StringField $expression;
    public StringField $value;
    public BooleanField $casesensitive;
    public BooleanField $not;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->parent_model_class = 'HAProxyFrontend';
        $this->config_path = 'ha_acls/item';
        $this->verbose_name = 'HAProxy Frontend Access Control List';
        $this->packages = ['pfSense-pkg-haproxy'];
        $this->package_includes = ['haproxy/haproxy.inc', 'haproxy/haproxy_utils.inc'];
        $this->many = true;

        # Set model fields
        $this->name = new StringField(
            required: true,
            minimum_length: 2,
            validators: [new RegexValidator(pattern: '/[^a-zA-Z0-9\.\-_]/', invert: true)],
            help_text: 'The unique name for this frontend ACL.',
        );
        $this->expression = new StringField(
            required: true,
            choices: [
                'host_starts_with',
                'host_ends_with',
                'host_matches',
                'host_regex',
                'host_contains',
                'path_starts_with',
                'path_ends_with',
                'path_matches',
                'path_regex',
                'path_contains',
                'path_dir',
                'url_parameter',
                'ssl_c_verify_code',
                'ssl_c_verify',
                'ssl_c_ca_commonname',
                'source_ip',
                'backendservercount',
                'traffic_is_http',
                'traffic_is_ssl',
                'ssl_sni_matches',
                'ssl_sni_contains',
                'ssl_sni_starts_with',
                'ssl_sni_ends_with',
                'ssl_sni_regex',
                'custom',
            ],
            help_text: 'The expression to use to determine the match for this ACL.',
        );
        $this->value = new StringField(
            required: true,
            allow_empty: true,
            help_text: 'The value which indicates a match for this ACL.',
        );
        $this->casesensitive = new BooleanField(
            default: false,
            indicates_true: 'yes',
            help_text: 'Enables or disables case-sensitive matching for this ACL.',
        );
        $this->not = new BooleanField(
            default: false,
            indicates_true: 'yes',
            help_text: 'Enables or disables inverting the context of this ACL.',
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
