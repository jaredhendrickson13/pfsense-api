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
     * @var array $index An associative array that contains cache Model objects that have been loaded and
     * indexed by specific fields for quick lookup. Structured as:
     *
     * [Model class name] => [
     *     [index field name] => [
     *         [index field value] => Model object
     *     ]
     * ]
     */
    public static array $index = [];

    /**
     * @var array $cache An associative array that contains cached Model objects that have been loaded into memory as
     * a complete ModelSet of every object of that Model type. Structured as: [Model class name] => ModelSet object
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
     * Raises an error if the given Model object does not support being indexed by the specified field.
     * @param Model $model The Model object to check for uniqueness.
     * @param string $index_field The index field name to check for uniqueness.
     * @throws ServerError If the Model is attempting to be indexed by a non-unique field.
     * @throws ServerError If the Model is not many-enabled.
     */
    private static function ensure_model_supports_indexing(Model $model, string $index_field): void {
        # Only many-enabled Models can be indexed
        if (!$model->many) {
            throw new ServerError(
                message: "Cannot index Model class '" . $model->get_class_fqn() . 'because it is not many-enabled.',
                response_id: 'MODEL_CACHE_INDEX_FIELD_ON_NON_MANY_MODEL',
            );
        }

        # Models with parent model classes cannot be indexed
        if ($model->parent_model_class) {
            throw new ServerError(
                message: "Cannot index Model class '" .
                    $model->get_class_fqn() .
                    "' because it has a parent model class '" .
                    $model->parent_model_class .
                    "'.",
                response_id: 'MODEL_CACHE_INDEX_FIELD_ON_PARENTED_MODEL',
            );
        }

        # If indexing by 'id', it's always unique
        if ($index_field === 'id') {
            return;
        }

        # Check if the index field is unique on the Model object
        if (!$model->$index_field->unique) {
            throw new ServerError(
                message: "Cannot index Model class '" .
                    $model->get_class_fqn() .
                    "' by non-unique field " .
                    "'$index_field'.",
                response_id: 'MODEL_CACHE_INDEX_FIELD_NOT_UNIQUE',
            );
        }
    }

    /**
     * Indexes the ModelSet cache for a given Model class by a specified field. This method will populate
     * the $index array for the specified Model class and index field.
     * @param string $model_class The Model class name whose ModelSet is to be indexed. If no cached ModelSet exists,
     * one will be created by reading all Model objects from the data source.
     * @param string $index_field The field name to index the Model objects by.
     * @return array An associative array of indexed Model objects.
     */
    public static function index_modelset_by_field(string $model_class, string $index_field): array {
        # First, check if this Model class has already been indexed by this field
        if (isset(self::$index[$model_class][$index_field])) {
            return self::$index[$model_class][$index_field];
        }

        # Ensure the Model can be indexed by the specified field
        $model = new $model_class();
        self::ensure_model_supports_indexing($model, $index_field);

        # Fetch or create the ModelSet for this Model class
        $model_set = $model->read_all();

        # Create an associative array to hold the indexed Model objects
        $indexed_models = [];

        # Index each Model object in the ModelSet by the specified field
        foreach ($model_set->model_objects as $model) {
            $index_value = $index_field === 'id' ? $model->id : $model->$index_field->value;
            $indexed_models[$index_value] = $model;
        }

        # Store the indexed models in the ModelCache index and return them
        self::$index[$model_class][$index_field] = $indexed_models;
        return $indexed_models;
    }

    /**
     * Checks if a given Model class has a cached Model object by its index field/value.
     * @param string $model_class The Model class name to check in the cache.
     * @param string $index_field The index field name to look up the Model object by
     * @param mixed $index_value The index field value to look up the Model object by
     * @return bool True if a cached Model object exists for the specified Model class and index
     */
    public static function has_model(string $model_class, string $index_field = 'id', mixed $index_value = null): bool {
        # Cache is always a miss if the Model class is not indexed
        if (!self::$index[$model_class]) {
            return false;
        }

        # Cache is a hit for non-many enabled models if a single Model object is indexed under the Model class
        if (self::$index[$model_class] instanceof Model) {
            return true;
        }

        # Otherwise, cache is only a hit for many enabled models if the index field/value exists in the index
        return isset(self::$index[$model_class][$index_field][$index_value]);
    }

    /**
     * Fetches a cached Model object by its Model class name and index field/value.
     * @param string $model_class The Model class name to load from the cache.
     * @param string $index_field The index field name to look up the Model object by. Only for many enabled Models.
     * @param mixed $index_value The index field value to look up the Model object by. Only for many enabled Models.
     * @return Model The cached Model object
     * @throws NotFoundError If no cached Model object exists for the specified Model class and index.
     */
    public static function fetch_model(
        string $model_class,
        string $index_field = 'id',
        mixed $index_value = null,
    ): Model {
        if (!self::has_model($model_class, $index_field, $index_value)) {
            throw new NotFoundError(
                message: "No cached Model found for Model class '$model_class' with " .
                    "index field '$index_field' and index value '$index_value'.",
                response_id: 'MODEL_CACHE_MODEL_NOT_FOUND',
            );
        }

        # For many enabled models, return the Model object indexed under the specified index field and value
        if (isset(self::$index[$model_class][$index_field][$index_value])) {
            return self::$index[$model_class][$index_field][$index_value];
        }

        # Otherwise, return the Model object indexed under the Model class only
        return self::$index[$model_class];
    }

    /**
     * Clears the ModelCache of all cached ModelSets and indexed Model objects.
     */
    public static function clear(): void {
        self::$index = [];
        self::$cache = [];
    }
}
