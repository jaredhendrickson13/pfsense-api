<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Dispatchers\ACMEAccountKeyRegisterDispatcher;

class APIDispatchersACMEAccountKeyRegisterDispatcherTestCase extends TestCase
{
    public array $required_packages = ['pfSense-pkg-acme'];
    public array $package_includes = [
        'acme/acme.inc',
        'acme/acme_utils.inc',
        'acme/acme_htmllist.inc',
        'acme/pkg_acme_tabs.inc',
    ];

    /**
     * Ensure an error is thrown if an id argument is not provided.
     */
    public function test_no_id_provider_to_spawn_process(): void {
        $this->assert_throws_response(
            response_id: 'ACME_ACCOUNT_KEY_REGISTER_DISPATCHER_NO_ID_PROVIDED',
            code: 500,
            callable: function () {
                $dispatcher = new ACMEAccountKeyRegisterDispatcher();
                $dispatcher->process();
            }
        );
    }
}