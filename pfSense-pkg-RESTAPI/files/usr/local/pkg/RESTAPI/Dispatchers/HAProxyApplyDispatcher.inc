<?php

namespace RESTAPI\Dispatchers;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Dispatcher;

/**
 * Defines a Dispatcher for applying changes to the HAProxy settings.
 */
class HAProxyApplyDispatcher extends Dispatcher {
    public int $max_queue = 1; // Only allow one HAProxy process to run at a time
    protected array $required_packages = ['pfSense-pkg-haproxy'];
    protected array $package_includes = ['haproxy/haproxy.inc', 'haproxy/haproxy_utils.inc'];

    /**
     * Applies changes to the HAProxy global settings.
     */
    protected function _process(mixed ...$arguments): void {
        $msg = null; // Needs a variable to pass into haproxy_check_and_run()
        $result = haproxy_check_and_run($msg, true);

        if ($result) {
            unlink_if_exists('/var/run/haproxy.conf.dirty');
        }
    }
}
