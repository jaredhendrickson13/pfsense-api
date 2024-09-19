<?php

namespace RESTAPI\Caches;

require_once 'RESTAPI/autoloader.inc';

use RESTAPI\Core\Cache;
use RESTAPI\Core\Command;

/**
 * Defines a Cache that can be used to update the available pfSense-pkg-RESTAPI releases cache. This Cache
 * runs on a schedule to periodically update the releases cache in the background.
 */
class RESTAPIVersionReleasesCache extends Cache {
    const RELEASES_URL = 'https://api.github.com/repos/jaredhendrickson13/pfsense-api/releases';

    public int $timeout = 30;
    public string $schedule = 'hourly';

    /**
     * Retrieves available release information from external repos and updates the releases cache files.
     */
    public function get_data_to_cache(): array {
        # Retrieve the available releases from the GitHub API
        $releases_json = \RESTAPI\Core\Tools\http_request(url: self::RELEASES_URL, method: 'GET');
        $releases = json_decode($releases_json, true);

        # Return the fetched releases if in a valid format, otherwise retain the existing cache
        return is_array($releases) ? $releases : $this->read();
    }
}
