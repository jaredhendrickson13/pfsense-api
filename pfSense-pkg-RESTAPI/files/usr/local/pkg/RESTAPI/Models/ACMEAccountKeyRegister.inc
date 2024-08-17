<?php

namespace RESTAPI\Models;

use RESTAPI;
use RESTAPI\Core\Model;
use RESTAPI\Fields\ForeignModelField;
use RESTAPI\Responses\ServerError;

/**
 * Defines a Model used to register a given ACME account key with the ACME package.
 */
class ACMEAccountKeyRegister extends Model {
    public ForeignModelField $name;

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
        $this->name = new ForeignModelField(
            model_name: 'ACMEAccountKey',
            model_field: 'name',
            required: true,
            help_text: 'The name of the ACME account key to register.',
        );

        parent::__construct($id, $parent_id, $data, ...$options);
    }

    /**
     * Psuedo-method to act as the internal callable for this Model. There is no internal representation for this Model
     * since the registration status is not stored and works in real-time.
     */
    protected function get_internal(): array {
        return ["success" => null];
    }

    /**
     * Overrides the default _create method to do nothing since this Model since it does not write or store any data.
     */
    protected function _create(): void {
    }


    /**
     * Registers the ACME account key with the upstream ACME server
     */
    public function apply(): void {
        # Obtain the requested ACMEAccountKey model instance and the upstream CA URL
        $account_key = $this->name->get_related_model();
        $ca = $account_key->get_acme_server_url();

        # Register the account key
        $success = \pfsense_pkg\acme\registerAcmeAccountKey(
            name: $account_key->name->value,
            ca: $ca,
            key: $account_key->accountkey->value,
            email: $account_key->email->value
        );

        # Throw an error if the registration failed
        if (!$success) {
            throw new ServerError(
                message: "Failed to register the ACME account key with the upstream ACME server. You can view the log ".
                    "file at /tmp/acme/{$account_key->name->value}/acme_issuecert.log for more information.",
                response_id: "ACME_SERVER_ACCOUNT_KEY_REGISTER_FAILED"
            );
        }
    }
}
