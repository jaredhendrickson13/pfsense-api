<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\NotFoundError;
use RESTAPI\Responses\ServerError;

require_once 'RESTAPI/autoloader.inc';

/**
 * Defines a ModelCache object. A ModelCache is a collection of Models and ModelSets that have already been loaded and
 * indexed in memory. This class is responsible for ensuring that Model objects are only loaded once. This is
 * distinctly different from the Cache class, which is responsible for caching data to/from disk.
 */
class ModelCache {
    use BaseTraits;

    /**
     * @var self|null $instance The singleton instance of the ModelCache.
     */
    public static ?self $instance = null;

    /**
     * @var array $cache An associative array that contains cached ModelSet objects that have been loaded into memory.
     * This includes root ModelSets that contain all Model objects of a given Model class as well as indexed queries
     * that have already been run against those ModelSets. Structured as: [ModelSet signature => ModelSet object].
     * Please note that root ModelSets are indexed by their qualified Model class name only, whereas queried ModelSets
     * are indexed by the root signature followed by a series of query hashes separated by colons.
     */
    public static array $cache = [];

    /**
     * Retrieves the singleton instance of the ModelCache.
     * @return self The singleton instance of the ModelCache.
     */
    public static function get_instance(): self {
        if (self::$instance === null) {
            self::$instance = new self();
        }
        return self::$instance;
    }

    /**
     * Checks if a given Model class has a cached ModelSet.
     * @param string $model_class The Model class name to check in the cache.
     * @return bool True if a cached ModelSet exists for the specified Model class, false otherwise.
     */
    public static function has_modelset(string $model_class): bool {
        return isset(self::$cache[$model_class]);
    }

    /**
     * Caches a ModelSet in the ModelCache.
     * @param string $model_class The Model class name of the ModelSet to cache.
     * @param ModelSet $model_set The ModelSet object to cache.
     */
    public static function cache_modelset(string $model_class, ModelSet $model_set): void {
        self::$cache[$model_class] = $model_set;
    }

    /**
     * Fetches a cached ModelSet by its Model class name.
     * @param string $model_class The Model class name to load from the cache.
     * @return ModelSet The cached ModelSet
     * @throws NotFoundError If no cached ModelSet exists for the specified Model class.
     */
    public static function fetch_modelset(string $model_class): ModelSet {
        if (!self::has_modelset($model_class)) {
            throw new NotFoundError(
                message: "No cached ModelSet found for Model class '$model_class'.",
                response_id: 'MODEL_CACHE_MODELSET_NOT_FOUND',
            );
        }
        return self::$cache[$model_class];
    }

    /**
     * Clears the ModelCache of all cached ModelSets and indexed Model objects.
     */
    public static function clear(): void {
        self::$cache = [];
    }
}
