<?php

namespace RESTAPI\Models;

use RESTAPI\Core\Model;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Fields\StringField;

class ACMECertificateIssue extends Model {
    public ForeignModelField $certificate;
    public StringField $result;

    public function __construct(mixed $id = null, mixed $parent_id = null, mixed $data = [], mixed ...$options) {
        # Define model attributes
        $this->always_apply = true;
        $this->internal_callable = 'get_internal';
        $this->packages = ['pfSense-pkg-acme'];
        $this->package_includes = [
            'acme/acme.inc',
            'acme/acme_utils.inc',
            'acme/acme_htmllist.inc',
            'acme/pkg_acme_tabs.inc',
        ];

        # Define model Fields
        $this->certificate = new ForeignModelField(
            model_name: 'ACMECertificate',
            model_field: 'name',
            required: true,
            help_text: 'The name of the ACME certificate to be issued.',
        );
        $this->result = new StringField(
            default: null,
            allow_null: true,
            read_only: true,
            help_text: 'The output result of the acme.sh issue command.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Psuedo-method to get the internal data for the model. This Model has no internal representation because it only
     * works with the acme.sh command line tool in real-time.
     */
    public function get_internal(): array {
        return [];
    }

    /**
     * Initiates the ACME certificate issue process.
     */
    public function _create(): void {
        # pfSense prints the output of the acme.sh instead of returning it, start output buffering to capture it
        ob_start();

        # Issue the ACME certificate
        \pfsense_pkg\acme\issue_certificate(id: $this->certificate->value, force: true, renew: false);

        # Obtain the result of the ACME certificate issue
        $this->result->value = ob_get_clean();
    }
}
