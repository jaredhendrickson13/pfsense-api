<?php

namespace RESTAPI\Dispatchers;

use RESTAPI\Core\Dispatcher;
use RESTAPI\Models\ACMEAccountKey;
use RESTAPI\Responses\ServerError;

/**
 * Defines a Dispatcher for issuing a new ACME certificate.
 */
class ACMECertificateIssueDispatcher extends Dispatcher {
    public array $required_packages = ['pfSense-pkg-acme'];
    public array $package_includes = [
        'acme/acme.inc',
        'acme/acme_utils.inc',
        'acme/acme_htmllist.inc',
        'acme/pkg_acme_tabs.inc',
    ];

    /**
     * Initiates the process of issuing a new ACME certificate.
     */
    protected function _process(mixed ...$arguments): void {
        # Throw an error if a certificate argument was not provided
        if (!isset($arguments['certificate'])) {
            throw new ServerError(
                message: 'Failed to spawn new ACME certificate issue process: No certificate name was provided.',
                response_id: "ACME_CERTIFICATE_ISSUE_DISPATCHER_NO_CERTIFICATE_PROVIDED",
            );
        }

        # Replace the issuance result file
        $this->set_issuance_result(
            cert: $arguments['certificate'],
            last_updated: time(),
            status: 'pending',
            result_log: '',
        );

        # pfSense prints the output of the acme.sh instead of returning it, start output buffering to capture it
        ob_start();

        # Issue the ACME certificate
        \pfsense_pkg\acme\issue_certificate(id: $arguments['certificate'], force: true, renew: false);

        # Log the results of the ACME certificate issue
        $result = ob_get_clean();

        # Replace the issuance result file
        $this->set_issuance_result(
            cert: $arguments['certificate'],
            last_updated: time(),
            status: 'completed',
            result_log: $result,
        );

        # Wait a bit to ensure the issue log is written before proceeding
        sleep(1);
    }

    /**
     * Sets the .issuance_result.json file for the given certificate to the given status and result log.
     */
    private function set_issuance_result(string $cert, int $last_updated, string $status, string $result_log): void {
        file_put_contents(
            "/tmp/.RESTAPI.acme.$cert.issuance_result.json",
            json_encode(
                ['last_updated' => $last_updated, 'status' => $status, 'result_log' => $result_log,]
            )
        );
    }
}
