<?php

namespace RESTAPI\Tests;

use RESTAPI\Core\TestCase;
use RESTAPI\Models\HAProxyApply;
use RESTAPI\Models\HAProxySettings;

class APIModelsHAProxyApplyTestCase extends TestCase {
    public array $required_packages = [['pfSense-pkg-haproxy', 'pfSense-pkg-haproxy-devel']];

    /**
     * Checks that the `applied` field correctly indicates whether HAProxy has pending configuration chagnes.
     */
    public function test_applied(): void {
        # Make a general change to HAProxy without applying
        $haproxy_settings = new HAProxySettings(maxconn: 120);
        $haproxy_settings->update();

        # Ensure the HAProxyApply model's `applied` field indicates there are pending changes
        $haproxy_apply = new HAProxyApply(async: false);
        $this->assert_is_false($haproxy_apply->applied->value);

        # Apply the changes and ensure the `applied` field now shows all changes are applied
        $haproxy_apply->apply();
        $haproxy_apply->from_internal();
        $this->assert_is_true($haproxy_apply->applied->value);
    }
}
