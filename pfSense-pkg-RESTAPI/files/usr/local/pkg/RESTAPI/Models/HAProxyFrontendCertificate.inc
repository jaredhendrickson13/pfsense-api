<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Dispatchers\HAProxyApplyDispatcher;
use RESTAPI\Fields\ForeignModelField;

/**
 * Defines a Model for HAProxy Frontend Access Control Lists.
 */
class HAProxyFrontendCertificate extends Model {
    public ForeignModelField $refid;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], ...$options) {
        # Set model attributes
        $this->parent_model_class = 'HAProxyFrontend';
        $this->config_path = 'ha_certificates/item';
        $this->verbose_name = 'HAProxy Frontend Certificates';
        $this->packages = ['pfSense-pkg-haproxy'];
        $this->package_includes = ['haproxy/haproxy.inc', 'haproxy/haproxy_utils.inc'];
        $this->many = true;

        $this->ssl_certificate = new ForeignModelField(
            model_name: 'Certificate',
            model_field: 'refid',
            default: null,
            allow_null: true,
            help_text: 'The default SSL/TLS certificate to use for this frontend.',
        ); 
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
