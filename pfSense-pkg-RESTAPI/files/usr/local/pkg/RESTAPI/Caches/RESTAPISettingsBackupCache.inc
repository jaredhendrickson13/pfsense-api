<?php

namespace RESTAPI\Caches;

use RESTAPI\Core\Cache;

/**
 * Defines a Cache for the REST API settings backups.
 */
class RESTAPISettingsBackupCache extends Cache {
    /**
     * Obtains the current REST API settings configuration.
     */
    public function get_data_to_cache(): array {
        return parent::get_data_to_cache();
    }
}
