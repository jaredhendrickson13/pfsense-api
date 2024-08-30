<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Models\ACMEAccountKey;
use RESTAPI\Responses\ServerError;

/**
 * Defines a Dispatcher for registering a given ACME account key with the ACME package.
 */
class ACMEAccountKeyRegisterDispatcher extends Dispatcher {
    public array $required_packages = ['pfSense-pkg-acme'];
    public array $package_includes = [
        'acme/acme.inc',
        'acme/acme_utils.inc',
        'acme/acme_htmllist.inc',
        'acme/pkg_acme_tabs.inc',
    ];

    /**
     * Registers a new account key with the ACME server.
     */
    protected function _process(mixed ...$arguments): void {
        # Throw an error if an id argument was not provided
        if (!isset($arguments['id'])) {
            throw new ServerError(
                message: 'Failed to spawn new ACME account key registration process: No id was provided.',
                response_id: 'ACME_ACCOUNT_KEY_REGISTER_DISPATCHER_NO_ID_PROVIDED',
            );
        }

        # Obtain the requested ACMEAccountKey model instance
        $account_key = new ACMEAccountKey(id: $arguments['id']);
        $ca = $account_key->get_acme_server_url();

        # Mark the account key as pending
        file_put_contents("/tmp/acme/{$account_key->name->value}/.registered", 'pending');

        # Register the account key
        $success = \pfsense_pkg\acme\registerAcmeAccountKey(
            name: $account_key->name->value,
            ca: $ca,
            key: $account_key->accountkey->value,
            email: $account_key->email->value,
        );

        # Throw an error if the registration failed
        if ($success) {
            file_put_contents("/tmp/acme/{$account_key->name->value}/.registered", 'registered');
        } else {
            file_put_contents("/tmp/acme/{$account_key->name->value}/.registered", 'failed');
        }

        # Wait a bit to ensure the registration status is updated
        sleep(3);
    }
}
