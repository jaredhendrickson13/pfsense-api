<?php

namespace RESTAPI\Caches;

require_once("RESTAPI/autoloader.inc");

use RESTAPI\Core\Cache;
use RESTAPI\Core\Command;

/**
 * Defines a Dispatcher that can be used to update the available pfSense-pkg-RESTAPI releases cache. This Dispatcher
 * runs on a schedule to periodically update the releases cache in the background.
 */
class RESTAPIVersionReleasesCache extends Cache
{
    const RELEASES_URL = "https://api.github.com/repos/jaredhendrickson13/pfsense-api/releases";

    public int $timeout = 30;
    public string $schedule = "hourly";

    /**
     * Retrieves available release information from external repos and updates the releases cache files.
     */
    public function get_data_to_cache(): array
    {
        # TODO: Change this to use PHP curl instead of a shell command
        $fetch_releases_cmd = "curl -s ".self::RELEASES_URL." -m $this->timeout";
        $releases_json = new Command($fetch_releases_cmd);
        return json_decode($releases_json->output, true);
    }
}
