<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Dispatchers\ACMEAccountKeyRegisterDispatcher;
use RESTAPI\Dispatchers\ACMECertificateIssueDispatcher;

class APIDispatchersACMECertificateIssueDispatcherTestCase extends TestCase {
    public array $required_packages = ['pfSense-pkg-acme'];
    public array $package_includes = [
        'acme/acme.inc',
        'acme/acme_utils.inc',
        'acme/acme_htmllist.inc',
        'acme/pkg_acme_tabs.inc',
    ];

    /**
     * Ensure an error is thrown if a certificate argument is not provided.
     */
    public function test_no_id_provider_to_spawn_process(): void {
        $this->assert_throws_response(
            response_id: 'ACME_CERTIFICATE_ISSUE_DISPATCHER_NO_CERTIFICATE_PROVIDED',
            code: 500,
            callable: function () {
                $dispatcher = new ACMECertificateIssueDispatcher();
                $dispatcher->process();
            },
        );
    }
}
