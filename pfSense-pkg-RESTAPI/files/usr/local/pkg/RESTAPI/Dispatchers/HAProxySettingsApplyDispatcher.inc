<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

class HAProxySettingsApplyDispatcher extends Dispatcher {
    public int $max_concurrency = 1; // Only allow one HAProxy process to run at a time
    protected array $required_packages = ['pfSense-pkg-haproxy'];
    protected array $package_includes = ["haproxy/haproxy.inc", "haproxy/haproxy_utils.inc"];

    /**
     * Applies changes to the HAProxy global settings.
     */
    protected function _process(mixed ...$arguments): void {
        $msg = null; // Needs a variable to pass into haproxy_check_and_run()
        haproxy_check_and_run($msg, true);
    }
}
