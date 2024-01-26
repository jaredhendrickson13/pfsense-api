<?php

namespace RESTAPI\Core;

use RESTAPI\Responses\ServerError;
use RESTAPI\Responses\ValidationError;

require_once("RESTAPI/autoloader.inc");

/**
 * Creates a ModelSet object that acts as a container for multiple Model objects. This can be used to represent a
 * set of Model objects at once. Additionally, ModelSets can be further filtered using queries.
 */
class ModelSet {
    const QUERY_FILTERS = ["startswith", "endswith", "contains", "lt", "lte", "gt", "gte", "exact", "except", "has"];
    public array $model_objects;

    /**
     * Creates a ModelSet object that contains multiple Model objects.
     * @param array $model_objects An array of model objects to include in this model set
     */
    public function __construct(array $model_objects = []) {
        # Throw an error if any Models are not a Model object
        foreach ($model_objects as $model_object) {
            if (!is_object($model_object) or !in_array("RESTAPI\Core\Model", class_parents($model_object))) {
                throw new ServerError(
                    message: "Model sets can only contain model objects.",
                    response_id: "MODEL_SET_CONTAINS_NON_MODEL"
                );
            }
        }

        # Assign the model objects to this model set
        $this->model_objects = $model_objects;
    }

    /**
     * Checks if Model objects exist in this ModelSet. This helpful when determining if an API query found matches.
     * @return bool true if Model objects exist in this ModelSet, false if they do not.
     */
    public function exists() : bool {
        return count($this->model_objects) > 0;
    }

    /**
     * Returns the first Model object in the ModelSet. This is helpful when locating the first Model object that matched
     * a query.
     */
    public function first() : Model {
        # Ensure there is at least 1 Model object in this ModelSet
        if ($this->exists()) {
            # Return the first Model object in the ModelSet
            return $this->model_objects[0];
        }

        # Throw an error if there are no Model objects in this ModelSet
        throw new ServerError(
            message: "Cannot locate first Model object in a ModelSet with no Model objects",
            response_id: "MODELSET_FIRST_REQUESTED_WITH_NO_MODEL_OBJECTS"
        );
    }

    /**
     * Runs the to_representation() method for each model object within the model set and returns them all in an array.
     * @return array A serializable representation of all model objects within this model set.
     */
    public function to_representation() : array {
        return $this->run_model_method(method_name: "to_representation");
    }

    /**
     * Runs the to_internal() method for each model object within the model set and returns them all in an array.
     * @return array The internal configu values of all model objects within this model set.
     */
    public function to_internal() : array {
        return $this->run_model_method(method_name: "to_internal", use_id_as_key: true);

    }

    /**
     * Runs a specified Model object method for each model object within this ModelSet.
     * @param string $method_name The name of the Model method to run for each model object in this model set.
     * @param bool $use_id_as_key Use each Model object's `id` property value as the return array keys. 
     * @return array The return value of method call for each model object. Results are stored in the same order as
     * the model objects were passed in during construction.
     */
    private function run_model_method(string $method_name, bool $use_id_as_key = false) : array {
        # Variables
        $results = [];

        # Loop through each model object and call its to_representation() method
        foreach ($this->model_objects as $loop_count => $model_object) {
            $id = ($use_id_as_key) ? $model_object->id : $loop_count;
            $results[$id] = $model_object->$method_name();
        }

        return $results;
    }

    /**
     * Filters the ModelSet to only include Model objects that match a specific query.
     * @param array $query_params An associative array of query targets and values. The array key will be the query
     * target and the array value will be the query value.
     * @param array $excluded An array of field names to exclude from the query. This is helpful when
     * query data may have extra values that you do not want to include in the query.
     * @param mixed ...$vl_query_params Qyery parameters that are passed in using variable-length arguments
     * https://www.php.net/manual/en/functions.arguments.php#functions.variable-arg-list
     * @return ModelSet The filtered ModelSet object containing only Model objects that matched the query.
     */
    public function query(array $query_params=[], array $excluded=[], ...$vl_query_params) : ModelSet {
        # Variables
        $queried_model_set = [];
        
        # Merge the $query_params and any provided variable-length arguments into a single variable
        $query_params = array_merge($query_params, $vl_query_params);

        # Loop through each model object in the provided model set and check it against the query parameters
        foreach ($this->model_objects as $model_object) {
            # Assign variable to track matches
            $is_match = true;

            # Loop through each query parameter to determine if this object is a match
            foreach ($query_params as $query_target => $query_value) {
                # Loop variables
                $query_options = $this->get_query_options($query_target);
                $field = $query_options["field"];
                $filter = $query_options["filter"];

                # Skip this field if it's excluded
                if (in_array($field, $excluded)) {
                    continue;
                }

                # Ensure this object has the requested field
                if (!property_exists($model_object, $field)) {
                    $is_match = false;
                    break;
                }

                # Obtain the value for this field. If it is the ID, handle accordingly.
                $value = ($field === "id") ? $model_object->id : $model_object->$field->value;

                # Ensure the filter matches
                if (!$this->$filter($value, $query_value)) {
                    $is_match = false;
                    break;
                }
            }
            # Add this model object to the filtered model set if it matched the query
            if ($is_match) {
                $queried_model_set[] = $model_object;
            }
        }
        return new ModelSet($queried_model_set);
    }

    /**
     * Checks for query target options. Query target options allow for more specific queries by limiting the scope to
     * values that match a specific filter.
     * @param string $query_target The query target string (e.g. `field_name`, `field_name__contains`)
     * @return array An array of query target options found in the $query_target string.
     */
    private function get_query_options(string $query_target) : array {
        # Variables
        $query_options = ["field"=>$query_target, "filter"=>"exact"];

        # Check if the double-underscore indicator is present
        if (str_contains($query_target, "__")) {
            # Separate query target parameters by __ to determine query options
            $query_target = explode("__", $query_target,);

            # Check if a valid query filter was specified
            if (in_array(end($query_target), self::QUERY_FILTERS)) {
                # Store the filter and remove the last item of the array.
                $query_options["filter"] = array_pop($query_target);
            }

            # Add the remaining values as the query field
            $query_options["field"] = implode("__", $query_target);
        }

        return $query_options;
    }

    /**
     * Checks if a given value string starts with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value startswith $substr. `false` if it does not.
     */
    private static function startswith(mixed $value, mixed $substr) : bool {
        if (str_starts_with(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if a given value string ends with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value endswith $substr. `false` if it does not.
     */
    private static function endswith(mixed $value, mixed $substr) : bool {
        if (str_ends_with(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if a given value string contains with another substring.
     * @param mixed $value The object value to evaluate.
     * @param mixed $substr The substring to check for.
     * @return bool `true` if $value contains $substr. `false` if it does not.
     */
    private static function contains(mixed $value, mixed $substr) : bool {
        if (str_contains(strval($value), strval($substr))) {
            return true;
        }
        return false;
    }

    /**
     * Checks if a given value is within an array.
     * @param mixed $haystack The array being evaluated.
     * @param mixed $needle The value to check for within the $haystack.
     * @return bool true if the $needle is in the $haystack, false otherwise.
     * @throws ValidationError When $haystack is not an array
     */
    public function has(mixed $haystack, mixed $needle) : bool {
        # Throw a validation error if the $haystack is not an array
        if (!is_array($haystack)) {
            throw new ValidationError(
                message: "Query filter `has` can only be used on array Fields",
                response_id: "MODELSET_HAS_QUERY_MUST_BE_ARRAY_FIELD"
            );

        }

        return in_array($needle, $haystack, strict: true);
    }

    /**
     * Checks if two values are an exact match.
     * @param mixed $value1 The first value to evaluate.
     * @param mixed $value2 The second value to evaluate.
     * @return bool `true` if the $value1 and $value2 match exactly. `false` if they do not.
     */
    private static function exact(mixed $value1, mixed $value2) : bool {
        return ($value1 == $value2);
    }

    /**
     * Checks if two values are not equal to each other
     * @param mixed $value1 The first value to evaluate.
     * @param mixed $value2 The second value to evaluate.
     * @return bool `true` if the $value1 and $value2 do not match. `false` if they do not.
     */
    private static function except(mixed $value1, mixed $value2) : bool {
        return ($value1 !== $value2);
    }

    /**
     * Checks if a value is less than a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is less than.
     * @return bool `true` if $value is less than $limit. `false` if it is not.
     */
    private function lt(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) < floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is less than or equal to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is less than or equal to.
     * @return bool `true` if $value is less than or equal to $limit. `false` if it is not.
     */
    private function lte(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) <= floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is greater than to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is greater than.
     * @return bool `true` if $value is greater than $limit. `false` if it is not.
     */
    private function gt(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) > floatval($limit)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a value is greater than or equal to a specified limit.
     * @param mixed $value The value to evaluate.
     * @param mixed $limit The value to check if $value is greater than or equal to.
     * @return bool `true` if $value is greater than or equal to $limit. `false` if it is not.
     */
    private function gte(mixed $value, mixed $limit) : bool {
        if (is_numeric($value) and is_numeric($limit)) {
            if (floatval($value) >= floatval($limit)) {
                return true;
            }
        }
        return false;
    }
}
