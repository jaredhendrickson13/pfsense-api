<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\ServerError;

require_once("RESTAPI/Core/Dispatcher.inc");
require_once("RESTAPI/autoloader.inc");

/**
 * Defines a Cache object. Cache objects are a type of Dispatcher object that is intended to fetch data from an external source
 * and store the output to a local cache file that periodically gets updated. This is helpful when handling data that
 * may take a long time to populate, or data that may have upstream rate limits that need to be respected.
 */
class Cache extends Dispatcher
{
    /**
     * @const CACHE_DIR The absolute file path to the directory where cache files are stored.
     */
    const CACHE_DIR = "/usr/local/share/pfSense-pkg-RESTAPI/.cache/";

    /**
     * Overrides the Dispatcher's default __construct method to add custom behavior for Cache construction.
     * @param bool $async Set to `false` to prevent this Dispatcher from running the process in the background.
     */
    public function __construct(bool $async = true) {
        # Construct this object as if it were a normal Dispatcher class, but overwrite the schedule_command for Caches
        parent::__construct(async: $async);
        $this->schedule_command = "/usr/local/share/pfSense-pkg-RESTAPI/manage.php refreshcache $this->short_name";
    }

    /**
     * Fetches the data that should be stored in the cache file.
     * @returns array The data to store in the cache.
     * @throws ServerError When this method is called but the get_data_to_cache() method has not been overwritten by
     * the child class.
     */
    protected function get_data_to_cache(): array {
        throw new ServerError(
            message: "\RESTAPI\Core\Cache classes must provide an overwritten get_data_to_cache() method.",
            response_id: "CACHE_GET_DATA_TO_CACHE_MUST_BE_OVERWRITTEN"
        );
    }

    /**
     * Obtains the filepath of this object's cache file.
     * @returns string The absolute filepath of the cache file for this object.
     */
    final public function get_file_path(): string {
        return self::CACHE_DIR.$this->short_name.".json";
    }

    /**
     * Obtains the cached data from the cache file.
     * @returns array The cached data in an array representation.
     */
    final public function read(): array {
        $cache_json = file_get_contents($this->get_file_path());
        return json_decode($cache_json, associative: true);
    }

    /**
     * Refreshes the cache by fetching new data using the `get_data_to-cache()` method and saving it as a JSON file to
     * the cache directory.
     */
    final public function process(...$arguments): void {
        file_put_contents($this->get_file_path(), json_encode($this->get_data_to_cache()));
    }
}
